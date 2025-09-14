#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Remove(const boost::asio::yield_context& yield, KeyDTO data) {
  auto [key, version] = std::move(data);
  if (!version.has_value()) {
    version = version_generator_.Generate();
  }
  auto res = wal_writer_.Remove(yield, key, version.value());
  if (res.has_failure()) {
    return res;
  }
  storage_.Remove(std::move(key), std::move(version).value());
  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
