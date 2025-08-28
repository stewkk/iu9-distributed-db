#include <stewkk/db/views/update.hpp>

namespace stewkk::db::views {

void UpdateHandler(UpdateController& controller, UpdateRPC& rpc, UpdateRPC::Request& request,
                   const boost::asio::yield_context& yield) {
  auto got = controller.Update(logic::controllers::KwDTO{request.key(), request.value()});
  if (got.has_failure()) {
    auto& error = got.assume_error();
    if (error.Wraps(logic::result::ErrorType::kNotFound)) {
      rpc.finish_with_error(grpc::Status(grpc::NOT_FOUND, error.What()), yield);
      return;
    }

    rpc.finish_with_error(grpc::Status(grpc::UNKNOWN, "unknown error"), yield);
    return;
  }
  InsertRPC::Response response;
  rpc.finish(response, grpc::Status::OK, yield);
}

}  // namespace stewkk::db::views
