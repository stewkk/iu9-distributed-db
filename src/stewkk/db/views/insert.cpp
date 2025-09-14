#include <stewkk/db/views/insert.hpp>

namespace stewkk::db::views {

logic::result::Result<InsertRPC::Response> InsertHandler(InsertController& controller,
                                                         InsertRPC& rpc,
                                                         InsertRPC::Request& request,
                                                         const boost::asio::yield_context& yield) {
  auto got = controller.Insert(
      yield, logic::controllers::KwDTO{request.key(), request.value(), request.version()});
  if (got.has_failure()) {
    return logic::result::WrapError(std::move(got), "insert controller failed");
  }
  InsertRPC::Response response;
  return response;
}

}  // namespace stewkk::db::views
