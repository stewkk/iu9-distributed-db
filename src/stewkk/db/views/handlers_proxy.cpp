#include <stewkk/db/views/handlers_proxy.hpp>

#include <stewkk/db/views/get.hpp>
#include <stewkk/db/views/insert.hpp>
#include <stewkk/db/views/remove.hpp>

namespace stewkk::db::views {

HandlersProxy::HandlersProxy(logic::controllers::Controller controller)
    : controller_(std::move(controller)) {}

void HandlersProxy::InsertHandler(InsertRPC& rpc, InsertRPC::Request& request,
                                  const boost::asio::yield_context& yield) {
  ::stewkk::db::views::InsertHandler(controller_, rpc, request, yield);
}

void HandlersProxy::GetHandler(GetRPC& rpc, GetRPC::Request& request,
                               const boost::asio::yield_context& yield) {
  ::stewkk::db::views::GetHandler(controller_, rpc, request, yield);
}

void HandlersProxy::RemoveHandler(RemoveRPC& rpc, RemoveRPC::Request& request,
                                  const boost::asio::yield_context& yield) {
  ::stewkk::db::views::RemoveHandler(controller_, rpc, request, yield);
}

}  // namespace stewkk::db::views
