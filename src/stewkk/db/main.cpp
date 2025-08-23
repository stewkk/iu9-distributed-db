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
#include <absl/log/log.h>

#include <api.grpc.pb.h>

#include <stewkk/db/views/add_handlers.hpp>

ABSL_FLAG(uint16_t, port, 50051, "Server port for the db");

void RunServer(uint16_t port) {
  auto server_address = std::format("0.0.0.0:{}", port);
  Db::AsyncService service;
  const auto thread_count = std::thread::hardware_concurrency() - 1;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  agrpc::GrpcContext grpc_context{builder.AddCompletionQueue(), thread_count};
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

  boost::thread_group threads;
  for (int i = 0; i < thread_count; ++i) {
    threads.create_thread([&grpc_context]() { grpc_context.run(); });
  }

  stewkk::db::views::AddHandlers(grpc_context, service);

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
