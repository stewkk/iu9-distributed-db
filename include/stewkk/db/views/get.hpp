#pragma once

#include <agrpc/server_rpc.hpp>
#include <boost/asio/spawn.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::views {

using GetRPC = agrpc::ServerRPC<&Db::AsyncService::RequestGet>;

void GetHandler(GetRPC& rpc, GetRPC::Request& request, const boost::asio::yield_context& yield);

}  // namespace stewkk::db::views
