#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Insert(KwDTO data) {
  auto [key, value] = data;
  storage_.Insert(models::storage::KwPair{std::move(key), std::move(value)});
  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
