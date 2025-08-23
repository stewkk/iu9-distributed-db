#include <stewkk/db/views/add_handlers.hpp>

#include <stewkk/db/views/get.hpp>
#include <stewkk/db/views/insert.hpp>
#include <stewkk/db/views/remove.hpp>
#include <stewkk/db/views/rethrow.hpp>
#include <stewkk/db/views/update.hpp>

namespace stewkk::db::views {

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
