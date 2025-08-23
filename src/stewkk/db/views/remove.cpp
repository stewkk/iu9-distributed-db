#include <stewkk/db/views/remove.hpp>

namespace stewkk::db::views {

void RemoveHandler(RemoveRPC& rpc, RemoveRPC::Request& request,
                   const boost::asio::yield_context& yield) {
  RemoveRPC::Response response;
  rpc.finish(response, grpc::Status::OK, yield);
}

}  // namespace stewkk::db::views
