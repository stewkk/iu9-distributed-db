#pragma once

#include <functional>

#include <stewkk/db/logic/coordination/zookeeper.hpp>
#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/views/get.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::views {

template <typename Controller, typename RPC>
std::function<logic::result::Result<typename RPC::Response>(
    Controller&, RPC&, typename RPC::Request&, const boost::asio::yield_context&)>
WithIsMasterMiddleware(
    std::function<logic::result::Result<typename RPC::Response>(
        Controller&, RPC&, typename RPC::Request&, const boost::asio::yield_context&)>
        handler) {
  return [handler](Controller& controller, RPC& rpc, RPC::Request& request,
                   const boost::asio::yield_context& yield) {
    if (request.source() == iu9db::Source::SOURCE_NODE) {
      return handler(controller, rpc, request, yield);
    }
    if (!logic::coordination::IsMaster()) {
      return logic::result::Result<typename RPC::Response>(
          logic::result::MakeError("node is not master"));
    }
    auto res = handler(controller, rpc, request, yield);
    if (!logic::coordination::IsMaster()) {
      return logic::result::Result<typename RPC::Response>(
          logic::result::MakeError("node is not master"));
    }
    return res;
  };
}

using GetHandlerType = decltype(GetHandler);

template <> std::function<GetHandlerType> WithIsMasterMiddleware<GetController, GetRPC>(
    std::function<GetHandlerType> handler);

}  // namespace stewkk::db::views
