#include <stewkk/db/views/handlers_proxy.hpp>

#include <stewkk/db/views/error_handling_middleware.hpp>
#include <stewkk/db/views/get.hpp>
#include <stewkk/db/views/insert.hpp>
#include <stewkk/db/views/is_master_middleware.hpp>
#include <stewkk/db/views/remove.hpp>

namespace stewkk::db::views {

HandlersProxy::HandlersProxy(logic::controllers::Controller controller)
    : controller_(std::move(controller)) {}

void HandlersProxy::InsertHandler(InsertRPC& rpc, InsertRPC::Request& request,
                                  const boost::asio::yield_context& yield) {
  (WithErrorHandlingMiddleware(
      WithIsMasterMiddleware<logic::controllers::InsertController, InsertRPC>(
          ::stewkk::db::views::InsertHandler)))(controller_, rpc, request, yield);
}

void HandlersProxy::GetHandler(GetRPC& rpc, GetRPC::Request& request,
                               const boost::asio::yield_context& yield) {
  (WithErrorHandlingMiddleware(WithIsMasterMiddleware<logic::controllers::GetController, GetRPC>(
      ::stewkk::db::views::GetHandler)))(controller_, rpc, request, yield);
}

void HandlersProxy::RemoveHandler(RemoveRPC& rpc, RemoveRPC::Request& request,
                                  const boost::asio::yield_context& yield) {
  (WithErrorHandlingMiddleware(WithIsMasterMiddleware<RemoveController, RemoveRPC>(
      ::stewkk::db::views::RemoveHandler)))(controller_, rpc, request, yield);
}

}  // namespace stewkk::db::views
