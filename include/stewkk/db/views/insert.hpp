#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/controllers/insert.hpp>
#include <stewkk/db/views/request_types.hpp>

namespace stewkk::db::views {

using logic::controllers::InsertController;

logic::result::Result<InsertRPC::Response> InsertHandler(InsertController& controller,
                                                         InsertRPC& rpc,
                                                         InsertRPC::Request& request,
                                                         const boost::asio::yield_context& yield);

}  // namespace stewkk::db::views
