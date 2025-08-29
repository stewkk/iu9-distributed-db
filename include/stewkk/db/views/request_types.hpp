#pragma once

#include <agrpc/server_rpc.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::views {

using RemoveRPC = agrpc::ServerRPC<&Db::AsyncService::RequestRemove>;
using InsertRPC = agrpc::ServerRPC<&Db::AsyncService::RequestInsert>;
using GetRPC = agrpc::ServerRPC<&Db::AsyncService::RequestGet>;
using ClearRPC = agrpc::ServerRPC<&Db::AsyncService::RequestClear>;

}  // namespace stewkk::db::views
