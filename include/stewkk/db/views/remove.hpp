#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/controllers/remove.hpp>
#include <stewkk/db/views/request_types.hpp>

namespace stewkk::db::views {

using logic::controllers::RemoveController;

logic::result::Result<RemoveRPC::Response> RemoveHandler(RemoveController& controller,
                                                         RemoveRPC& rpc,
                                                         RemoveRPC::Request& request,
                                                         const boost::asio::yield_context& yield);

}  // namespace stewkk::db::views
