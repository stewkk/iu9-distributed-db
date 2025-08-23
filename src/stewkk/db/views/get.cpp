#include <stewkk/db/views/get.hpp>

namespace stewkk::db::views {

void GetHandler(GetRPC& rpc, GetRPC::Request& request, const boost::asio::yield_context& yield) {
  GetRPC::Response response;
  response.set_value("blabla");
  rpc.finish(response, grpc::Status::OK, yield);
}

}  // namespace stewkk::db::views
