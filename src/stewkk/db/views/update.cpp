#include <stewkk/db/views/update.hpp>

namespace stewkk::db::views {

void UpdateHandler(UpdateController& controller, UpdateRPC& rpc, UpdateRPC::Request& request,
                   const boost::asio::yield_context& yield) {
  UpdateRPC::Response response;
  rpc.finish(response, grpc::Status::OK, yield);
}

}  // namespace stewkk::db::views
