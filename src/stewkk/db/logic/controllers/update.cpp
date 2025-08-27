#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Update(KwDTO data) {
  auto [key, value] = data;
  auto res = storage_.Update(models::storage::KwPair{std::move(key), std::move(value)});
  if (res.has_failure()) {
    return result::WrapError(std::move(res), "failed to update key in memstorage");
  }
  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
