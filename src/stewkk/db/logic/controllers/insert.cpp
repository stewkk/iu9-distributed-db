#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Insert(const boost::asio::yield_context& yield, KwDTO data) {
  if (!data.version.has_value()) {
    data.version = version_generator_.Generate();
  }

  auto [key, value, version] = std::move(data);
  models::storage::KwPair kw_pair{std::move(key), std::move(value), std::move(version).value()};
  auto res = wal_writer_.Insert(yield, kw_pair);
  if (res.has_failure()) {
    return res;
  }
  storage_.Insert(std::move(kw_pair));

  TriggerSwapToPersistentStorage();
  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
