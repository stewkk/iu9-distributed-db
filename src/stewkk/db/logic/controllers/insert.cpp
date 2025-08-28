#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Insert(const boost::asio::yield_context& yield, KwDTO data) {
  auto [key, value] = std::move(data);
  models::storage::KwPair kw_pair{std::move(key), std::move(value)};
  auto res = wal_writer_.Insert(yield, kw_pair);
  if (res.has_failure()) {
    return res;
  }
  storage_.Insert(std::move(kw_pair));
  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
