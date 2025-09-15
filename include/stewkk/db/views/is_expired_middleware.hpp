#pragma once

#include <functional>

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/coordination/zookeeper.hpp>
#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::views {

template <typename Controller, typename RPC>
std::function<logic::result::Result<typename RPC::Response>(
    Controller&, RPC&, typename RPC::Request&, const boost::asio::yield_context&)>
WithIsExpiredMiddleware(
    std::function<logic::result::Result<typename RPC::Response>(
        Controller&, RPC&, typename RPC::Request&, const boost::asio::yield_context&)>
        handler) {
  return [handler](Controller& controller, RPC& rpc, RPC::Request& request,
                   const boost::asio::yield_context& yield) {
    if (logic::coordination::IsConnectionExpired()) {
      return logic::result::Result<typename RPC::Response>(
          logic::result::MakeError<logic::result::ErrorType::kNotConnected>(
              "node is not connected to cluster"));
    }
    auto res = handler(controller, rpc, request, yield);
    if (logic::coordination::IsConnectionExpired()) {
      return logic::result::Result<typename RPC::Response>(
          logic::result::MakeError<logic::result::ErrorType::kNotConnected>(
              "node is not connected to cluster"));
    }
    return res;
  };
}

}  // namespace stewkk::db::views
