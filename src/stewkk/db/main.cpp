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
#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <absl/log/log.h>
#include <absl/log/log_sink_registry.h>

#include <api.grpc.pb.h>

#include <stewkk/db/logic/log/file_log_sink.hpp>
#include <stewkk/db/logic/recovery/swappable_wal_writer_impl.hpp>
#include <stewkk/db/logic/recovery/wal_reader.hpp>
#include <stewkk/db/logic/storage/persistent_collection.hpp>
#include <stewkk/db/logic/storage/swappable_memstorage.hpp>
#include <stewkk/db/views/register_handlers.hpp>

ABSL_FLAG(uint16_t, port, 50051, "Server port for the db");
ABSL_FLAG(std::string, logfile, "", "Where to write logs. Default is stderr");
ABSL_FLAG(std::string, datadir, "/tmp/iu9-distributed-db", "Data directory");
ABSL_FLAG(uint16_t, debug_level, 0, "Debug logs level");

void RunServer(uint16_t port) {
  auto server_address = std::format("0.0.0.0:{}", port);
  iu9db::Db::AsyncService service;
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

  auto storages = stewkk::db::logic::recovery::InitializeStorages(grpc_context.get_executor());
  if (storages.has_failure()) {
    LOG(FATAL) << "failed to initialize storages: " << storages.assume_error().What();
  }
  auto [persistent, storage, wal_writer] = std::move(storages).assume_value();
  stewkk::db::views::HandlersProxy handlers(stewkk::db::logic::controllers::Controller{
      storage, wal_writer, wal_writer, persistent, grpc_context.get_executor()});
  stewkk::db::views::RegisterHandlers(handlers, grpc_context, service);

  LOG(INFO) << "Server listening on " << server_address;
  grpc_context.run();
  threads.join_all();
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::SetGlobalVLogLevel(absl::GetFlag(FLAGS_debug_level));
  absl::InitializeLog();
  auto logfile = absl::GetFlag(FLAGS_logfile);
  std::unique_ptr<stewkk::db::logic::log::FileLogSink> sink;
  if (!logfile.empty()) {
    sink = stewkk::db::logic::log::FileLogSink::New(logfile).value();
    absl::AddLogSink(sink.get());
  }
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  stewkk::db::logic::filesystem::SetDataDir(absl::GetFlag(FLAGS_datadir));
  RunServer(absl::GetFlag(FLAGS_port));
  return 0;
}
