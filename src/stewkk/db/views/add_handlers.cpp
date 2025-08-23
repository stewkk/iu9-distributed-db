#include <stewkk/db/views/add_handlers.hpp>

namespace stewkk::db::views {

namespace {

struct RethrowFirstArg {
  template <class... T> void operator()(std::exception_ptr ep, T&&...) {
    if (ep) {
      std::rethrow_exception(ep);
    }
  }

  template <class... T> void operator()(T&&...) {}
};

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

}  // namespace

void AddHandlers(agrpc::GrpcContext& grpc_context, Db::AsyncService& service) {
  agrpc::register_yield_rpc_handler<InsertRPC>(grpc_context, service, InsertHandler,
                                               RethrowFirstArg{});
  agrpc::register_yield_rpc_handler<UpdateRPC>(grpc_context, service, UpdateHandler,
                                               RethrowFirstArg{});
  agrpc::register_yield_rpc_handler<RemoveRPC>(grpc_context, service, RemoveHandler,
                                               RethrowFirstArg{});
  agrpc::register_yield_rpc_handler<GetRPC>(grpc_context, service, GetHandler, RethrowFirstArg{});
}

}  // namespace stewkk::db::views
