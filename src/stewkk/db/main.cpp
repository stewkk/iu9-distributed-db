#include <cstdint>
#include <format>
#include <memory>
#include <thread>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <agrpc/register_yield_rpc_handler.hpp>

#include <boost/thread.hpp>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/log/initialize.h>
#include <absl/log/log.h>

#include <api.grpc.pb.h>

using InsertRPC = agrpc::ServerRPC<&Db::AsyncService::RequestInsert>;
using UpdateRPC = agrpc::ServerRPC<&Db::AsyncService::RequestUpdate>;
using RemoveRPC = agrpc::ServerRPC<&Db::AsyncService::RequestRemove>;
using GetRPC = agrpc::ServerRPC<&Db::AsyncService::RequestGet>;

void InsertHandler(InsertRPC& rpc, InsertRPC::Request& request,
                   const boost::asio::yield_context& yield) {
  InsertRPC::Response response;
  rpc.finish(response, grpc::Status::OK, yield);
}

void UpdateHandler(UpdateRPC& rpc, UpdateRPC::Request& request,
                   const boost::asio::yield_context& yield) {
  UpdateRPC::Response response;
  rpc.finish(response, grpc::Status::OK, yield);
}

void RemoveHandler(RemoveRPC& rpc, RemoveRPC::Request& request,
                   const boost::asio::yield_context& yield) {
  RemoveRPC::Response response;
  rpc.finish(response, grpc::Status::OK, yield);
}

void GetHandler(GetRPC& rpc, GetRPC::Request& request, const boost::asio::yield_context& yield) {
  GetRPC::Response response;
  response.set_value("blabla");
  rpc.finish(response, grpc::Status::OK, yield);
}

ABSL_FLAG(uint16_t, port, 50051, "Server port for the db");

struct RethrowFirstArg {
  template <class... T> void operator()(std::exception_ptr ep, T&&...) {
    if (ep) {
      std::rethrow_exception(ep);
    }
  }

  template <class... T> void operator()(T&&...) {}
};

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

  agrpc::register_yield_rpc_handler<InsertRPC>(grpc_context, service, InsertHandler,
                                               RethrowFirstArg{});
  agrpc::register_yield_rpc_handler<UpdateRPC>(grpc_context, service, UpdateHandler,
                                               RethrowFirstArg{});
  agrpc::register_yield_rpc_handler<RemoveRPC>(grpc_context, service, RemoveHandler,
                                               RethrowFirstArg{});
  agrpc::register_yield_rpc_handler<GetRPC>(grpc_context, service, GetHandler, RethrowFirstArg{});

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
