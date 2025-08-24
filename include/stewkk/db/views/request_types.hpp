#pragma once

#include <agrpc/server_rpc.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::views {

using UpdateRPC = agrpc::ServerRPC<&Db::AsyncService::RequestUpdate>;
using RemoveRPC = agrpc::ServerRPC<&Db::AsyncService::RequestRemove>;
using InsertRPC = agrpc::ServerRPC<&Db::AsyncService::RequestInsert>;
using GetRPC = agrpc::ServerRPC<&Db::AsyncService::RequestGet>;

}  // namespace stewkk::db::views
