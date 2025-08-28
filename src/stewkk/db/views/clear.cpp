#include <stewkk/db/views/clear.hpp>

namespace stewkk::db::views {

void ClearHandler(ClearController& controller, ClearRPC& rpc, ClearRPC::Request& request,
                  const boost::asio::yield_context& yield) {
  auto got = controller.Clear();
  // TODO: вот тут хочу превращать ошибку из result в error code, для этого нужна функция типа
  // Unwrap()
  if (got.has_failure()) {
    rpc.finish_with_error(grpc::Status(grpc::UNKNOWN, "unknown error"), yield);
    return;
  }
  InsertRPC::Response response;
  rpc.finish(response, grpc::Status::OK, yield);
}

}  // namespace stewkk::db::views
