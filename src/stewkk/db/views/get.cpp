#include <stewkk/db/views/get.hpp>

namespace stewkk::db::views {

using logic::controllers::GetRequestDTO;

logic::result::Result<GetRPC::Response> GetHandler(GetController& controller, GetRPC& rpc,
                                                   GetRPC::Request& request,
                                                   const boost::asio::yield_context& yield) {
  auto got = controller.Get(yield, GetRequestDTO{request.key()});
  if (got.has_failure()) {
    return logic::result::WrapError(std::move(got), "get controller failed");
  }
  auto [value, version] = std::move(got).assume_value();
  GetRPC::Response response;
  response.set_value(std::move(value));
  response.set_version(std::move(version));
  return response;
}

}  // namespace stewkk::db::views
