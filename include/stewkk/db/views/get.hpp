#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/controllers/get.hpp>
#include <stewkk/db/views/request_types.hpp>

namespace stewkk::db::views {

using logic::controllers::GetController;

void GetHandler(GetController& controller, GetRPC& rpc, GetRPC::Request& request,
                const boost::asio::yield_context& yield);

}  // namespace stewkk::db::views
