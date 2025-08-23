#pragma once

#include <agrpc/server_rpc.hpp>
#include <boost/asio/spawn.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::views {

using InsertRPC = agrpc::ServerRPC<&Db::AsyncService::RequestInsert>;

void InsertHandler(InsertRPC& rpc, InsertRPC::Request& request,
                   const boost::asio::yield_context& yield);

}  // namespace stewkk::db::views
