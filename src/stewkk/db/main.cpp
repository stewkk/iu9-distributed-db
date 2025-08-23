#include <cstdint>
#include <format>
#include <memory>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <agrpc/register_yield_rpc_handler.hpp>

#include <boost/asio/detached.hpp>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/log/initialize.h>
#include <absl/log/log.h>

#include <example.grpc.pb.h>

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
  Greeter::AsyncService service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  agrpc::GrpcContext grpc_context{builder.AddCompletionQueue()};
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  LOG(INFO) << "Server listening on " << server_address;

  using RPC = agrpc::ServerRPC<&Greeter::AsyncService::RequestSayHello>;
  agrpc::register_yield_rpc_handler<RPC>(
      grpc_context, service,
      [](RPC& rpc, RPC::Request& request, const boost::asio::yield_context& yield) {
        RPC::Response response;
        std::string prefix("Hello ");
        response.set_message(prefix + request.name());
        rpc.finish(response, grpc::Status::OK, yield);
      },
      RethrowFirstArg{});

  grpc_context.run();
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();
  RunServer(absl::GetFlag(FLAGS_port));
  return 0;
}
