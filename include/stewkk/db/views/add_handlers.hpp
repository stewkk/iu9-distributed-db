#pragma once

#include <agrpc/register_yield_rpc_handler.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::views {

void AddHandlers(agrpc::GrpcContext& grpc_context, Db::AsyncService& service);

}  // namespace stewkk::db::views
