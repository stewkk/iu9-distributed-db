#include <stewkk/db/views/register_handlers.hpp>

#include <functional>

#include <stewkk/db/views/rethrow.hpp>

namespace stewkk::db::views {

void RegisterHandlers(HandlersProxy& handlers, agrpc::GrpcContext& grpc_context,
                      Db::AsyncService& service) {
  using namespace std::placeholders;

  agrpc::register_yield_rpc_handler<InsertRPC>(
      grpc_context, service, std::bind(&HandlersProxy::InsertHandler, handlers, _1, _2, _3),
      RethrowFirstArg{});
  agrpc::register_yield_rpc_handler<UpdateRPC>(
      grpc_context, service, std::bind(&HandlersProxy::UpdateHandler, handlers, _1, _2, _3),
      RethrowFirstArg{});
  agrpc::register_yield_rpc_handler<RemoveRPC>(
      grpc_context, service, std::bind(&HandlersProxy::RemoveHandler, handlers, _1, _2, _3),
      RethrowFirstArg{});
  agrpc::register_yield_rpc_handler<GetRPC>(
      grpc_context, service, std::bind(&HandlersProxy::GetHandler, handlers, _1, _2, _3),
      RethrowFirstArg{});
}

}  // namespace stewkk::db::views
