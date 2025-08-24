#pragma once

#include <agrpc/register_yield_rpc_handler.hpp>

#include <api.grpc.pb.h>

#include <stewkk/db/views/handlers_proxy.hpp>

namespace stewkk::db::views {

void RegisterHandlers(HandlersProxy& handlers, agrpc::GrpcContext& grpc_context,
                      Db::AsyncService& service);

}  // namespace stewkk::db::views
