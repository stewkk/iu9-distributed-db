#include <stewkk/db/views/insert.hpp>

namespace stewkk::db::views {

void InsertHandler(InsertController& controller, InsertRPC& rpc, InsertRPC::Request& request,
                   const boost::asio::yield_context& yield) {
  auto got = controller.Insert(logic::controllers::KwDTO{request.key(), request.value()});
  if (got.has_failure()) {
    rpc.finish_with_error(grpc::Status(grpc::UNKNOWN, "unknown error"), yield);
    return;
  }
  InsertRPC::Response response;
  rpc.finish(response, grpc::Status::OK, yield);
}

}  // namespace stewkk::db::views
