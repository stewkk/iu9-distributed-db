#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/controllers/update.hpp>
#include <stewkk/db/views/request_types.hpp>

namespace stewkk::db::views {

using logic::controllers::UpdateController;

void UpdateHandler(UpdateController& controller, UpdateRPC& rpc, UpdateRPC::Request& request,
                   const boost::asio::yield_context& yield);

}  // namespace stewkk::db::views
