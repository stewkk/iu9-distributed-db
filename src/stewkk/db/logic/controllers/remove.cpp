#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Remove(const boost::asio::yield_context& yield, KeyDTO data) {
  auto [key] = std::move(data);
  auto res = wal_writer_.Remove(yield, key);
  if (res.has_failure()) {
    return res;
  }
  res = storage_.Remove(std::move(key));
  if (res.has_failure()) {
    return result::WrapError(std::move(res), "failed to remove value from memory storage");
  }
  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
