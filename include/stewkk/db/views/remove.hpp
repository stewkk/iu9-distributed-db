#pragma once

#include <agrpc/server_rpc.hpp>
#include <boost/asio/spawn.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::views {

using RemoveRPC = agrpc::ServerRPC<&Db::AsyncService::RequestRemove>;

void RemoveHandler(RemoveRPC& rpc, RemoveRPC::Request& request,
                   const boost::asio::yield_context& yield);

}  // namespace stewkk::db::views
