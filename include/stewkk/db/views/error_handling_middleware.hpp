#pragma once

#include <functional>

#include <agrpc/server_rpc.hpp>
#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/views/get.hpp>
#include <stewkk/db/views/request_types.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::views {

template <typename Controller, typename RPC>
std::function<void(Controller&, RPC&, typename RPC::Request&, const boost::asio::yield_context&)>
WithErrorHandlingMiddleware(
    std::function<logic::result::Result<typename RPC::Response>(
        Controller&, RPC&, typename RPC::Request&, const boost::asio::yield_context&)>
        handler) {
  return [handler](Controller& controller, RPC& rpc, typename RPC::Request& request,
                   const boost::asio::yield_context& yield) {
    auto got = handler(controller, rpc, request, yield);
    if (got.has_failure()) {
      auto error = std::move(got).assume_error();
      if (error.Wraps(logic::result::ErrorType::kNotFound)) {
        rpc.finish_with_error(grpc::Status(grpc::NOT_FOUND, error.What()), yield);
        return;
      }

      rpc.finish_with_error(grpc::Status(grpc::UNKNOWN, "unknown error"), yield);
      return;
    }

    rpc.finish(std::move(got).assume_value(), grpc::Status::OK, yield);
  };
}

}  // namespace stewkk::db::views
