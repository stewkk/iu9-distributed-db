#pragma once

#include <agrpc/server_rpc.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::views {

using RemoveRPC = agrpc::ServerRPC<&iu9db::Db::AsyncService::RequestRemove>;
using InsertRPC = agrpc::ServerRPC<&iu9db::Db::AsyncService::RequestInsert>;
using GetRPC = agrpc::ServerRPC<&iu9db::Db::AsyncService::RequestGet>;

}  // namespace stewkk::db::views
