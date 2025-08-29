#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/controllers/controller.hpp>
#include <stewkk/db/views/request_types.hpp>

namespace stewkk::db::views {

class HandlersProxy {
public:
  explicit HandlersProxy(logic::controllers::Controller controller);

  void InsertHandler(InsertRPC& rpc, InsertRPC::Request& request,
                     const boost::asio::yield_context& yield);
  void GetHandler(GetRPC& rpc, GetRPC::Request& request, const boost::asio::yield_context& yield);
  void RemoveHandler(RemoveRPC& rpc, RemoveRPC::Request& request,
                     const boost::asio::yield_context& yield);
  void ClearHandler(ClearRPC& rpc, ClearRPC::Request& request,
                    const boost::asio::yield_context& yield);

private:
  logic::controllers::Controller controller_;
};

}  // namespace stewkk::db::views
