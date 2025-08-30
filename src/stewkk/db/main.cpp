#include <cstdint>
#include <format>
#include <memory>
#include <thread>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <agrpc/grpc_context.hpp>

#include <boost/thread.hpp>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/log/initialize.h>

#include <api.grpc.pb.h>

#include <stewkk/db/logic/recovery/swappable_wal_writer_impl.hpp>
#include <stewkk/db/logic/storage/persistent_collection.hpp>
#include <stewkk/db/logic/storage/swappable_memstorage.hpp>
#include <stewkk/db/views/register_handlers.hpp>

ABSL_FLAG(uint16_t, port, 50051, "Server port for the db");

void RunServer(uint16_t port) {
  auto server_address = std::format("0.0.0.0:{}", port);
  Db::AsyncService service;
  const auto thread_count = std::thread::hardware_concurrency();

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  agrpc::GrpcContext grpc_context{builder.AddCompletionQueue(), thread_count};
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

  boost::thread_group threads;
  for (int i = 0; i < thread_count - 1; ++i) {
    threads.create_thread([&grpc_context]() { grpc_context.run(); });
  }

  stewkk::db::logic::storage::PersistentStorageCollection persistent;
  stewkk::db::logic::storage::SwappableMemoryStorage storage;
  auto wal_writer
      = stewkk::db::logic::recovery::NewSwappableWalWriter(grpc_context.get_executor()).value();
  stewkk::db::views::HandlersProxy handlers(
      stewkk::db::logic::controllers::Controller{storage, wal_writer, persistent});
  stewkk::db::views::RegisterHandlers(handlers, grpc_context, service);

  LOG(INFO) << "Server listening on " << server_address;
  grpc_context.run();
  threads.join_all();
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();
  RunServer(absl::GetFlag(FLAGS_port));
  return 0;
}
