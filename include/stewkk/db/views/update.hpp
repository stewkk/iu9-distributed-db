#pragma once

#include <agrpc/server_rpc.hpp>
#include <boost/asio/spawn.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::views {

using UpdateRPC = agrpc::ServerRPC<&Db::AsyncService::RequestUpdate>;

void UpdateHandler(UpdateRPC& rpc, UpdateRPC::Request& request,
                   const boost::asio::yield_context& yield);

}  // namespace stewkk::db::views
