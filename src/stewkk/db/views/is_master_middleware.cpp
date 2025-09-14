#include <stewkk/db/views/is_master_middleware.hpp>

namespace stewkk::db::views {

template <> std::function<GetHandlerType> WithIsMasterMiddleware<GetController, GetRPC>(
    std::function<GetHandlerType> handler) {
  return [handler](logic::controllers::GetController& controller, GetRPC& rpc,
                   GetRPC::Request& request, const boost::asio::yield_context& yield) {
    if (request.source() == iu9db::Source::SOURCE_NODE) {
      return handler(controller, rpc, request, yield);
    }
    if (request.is_master() && !logic::coordination::IsMaster()) {
      return logic::result::Result<GetRPC::Response>(
          logic::result::MakeError("node is not master"));
    }
    auto res = handler(controller, rpc, request, yield);
    if (request.is_master() && !logic::coordination::IsMaster()) {
      return logic::result::Result<GetRPC::Response>(
          logic::result::MakeError("node is not master"));
    }
    return res;
  };
}

}  // namespace stewkk::db::views
