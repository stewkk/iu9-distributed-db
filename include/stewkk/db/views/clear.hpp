#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/controllers/clear.hpp>
#include <stewkk/db/views/request_types.hpp>

namespace stewkk::db::views {

using logic::controllers::ClearController;

void ClearHandler(ClearController& controller, ClearRPC& rpc, ClearRPC::Request& request,
                  const boost::asio::yield_context& yield);

}  // namespace stewkk::db::views
