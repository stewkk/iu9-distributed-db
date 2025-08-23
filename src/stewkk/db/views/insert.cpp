#include <stewkk/db/views/insert.hpp>

namespace stewkk::db::views {

void InsertHandler(InsertRPC& rpc, InsertRPC::Request& request,
                   const boost::asio::yield_context& yield) {
  InsertRPC::Response response;
  rpc.finish(response, grpc::Status::OK, yield);
}

}  // namespace stewkk::db::views
