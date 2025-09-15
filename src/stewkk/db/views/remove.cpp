#include <stewkk/db/views/remove.hpp>

namespace stewkk::db::views {

using models::dto::KeyDTO;

logic::result::Result<RemoveRPC::Response> RemoveHandler(RemoveController& controller,
                                                         RemoveRPC& rpc,
                                                         RemoveRPC::Request& request,
                                                         const boost::asio::yield_context& yield) {
  auto got = controller.Remove(yield, KeyDTO{request.key(), request.version()});
  if (got.has_failure()) {
    return logic::result::WrapError(std::move(got), "remove controller failed");
  }
  RemoveRPC::Response response;
  return response;
}

}  // namespace stewkk::db::views
