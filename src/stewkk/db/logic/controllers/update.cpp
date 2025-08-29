#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Update(const boost::asio::yield_context& yield, KwDTO data) {
  auto [key, value] = std::move(data);
  auto kw_pair = models::storage::KwPair{std::move(key), std::move(value)};
  auto res = wal_writer_.Update(yield, kw_pair);
  if (res.has_failure()) {
    return res;
  }
  res = storage_.Update(std::move(kw_pair));
  if (res.has_failure()) {
    return result::WrapError(std::move(res), "failed to update key in memstorage");
  }
  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
