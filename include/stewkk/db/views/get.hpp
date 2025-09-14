#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/controllers/get.hpp>
#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/views/request_types.hpp>

namespace stewkk::db::views {

using logic::controllers::GetController;

logic::result::Result<GetRPC::Response> GetHandler(GetController& controller, GetRPC& rpc,
                                                   GetRPC::Request& request,
                                                   const boost::asio::yield_context& yield);

}  // namespace stewkk::db::views
