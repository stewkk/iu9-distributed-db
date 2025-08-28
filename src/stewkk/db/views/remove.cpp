#include <stewkk/db/views/remove.hpp>

namespace stewkk::db::views {

using logic::controllers::KeyDTO;

void RemoveHandler(RemoveController& controller, RemoveRPC& rpc, RemoveRPC::Request& request,
                   const boost::asio::yield_context& yield) {
  auto got = controller.Remove(KeyDTO{request.key()});
  if (got.has_failure()) {
    auto& error = got.assume_error();
    if (error.Wraps(logic::result::ErrorType::kNotFound)) {
      rpc.finish_with_error(grpc::Status(grpc::NOT_FOUND, error.What()), yield);
      return;
    }

    rpc.finish_with_error(grpc::Status(grpc::UNKNOWN, "unknown error"), yield);
    return;
  }
  RemoveRPC::Response response;
  rpc.finish(response, grpc::Status::OK, yield);
}

}  // namespace stewkk::db::views
