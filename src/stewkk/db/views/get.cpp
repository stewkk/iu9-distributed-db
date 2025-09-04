#include <stewkk/db/views/get.hpp>

namespace stewkk::db::views {

using logic::controllers::KeyDTO;

void GetHandler(GetController& controller, GetRPC& rpc, GetRPC::Request& request,
                const boost::asio::yield_context& yield) {
  auto got = controller.Get(yield, KeyDTO{request.key()});
  if (got.has_failure()) {
    auto& error = got.assume_error();
    if (error.Wraps(logic::result::ErrorType::kNotFound)) {
      rpc.finish_with_error(grpc::Status(grpc::NOT_FOUND, error.What()), yield);
      return;
    }

    rpc.finish_with_error(grpc::Status(grpc::UNKNOWN, "unknown error"), yield);
    return;
  }
  auto& [value] = got.assume_value();
  GetRPC::Response response;
  response.set_value(std::move(value));
  rpc.finish(response, grpc::Status::OK, yield);
}

}  // namespace stewkk::db::views
