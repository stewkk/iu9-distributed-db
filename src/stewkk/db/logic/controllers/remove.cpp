#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Remove(KeyDTO data) {
  auto [key] = data;
  auto res = storage_.Remove(std::move(key));
  if (res.has_failure()) {
    return result::WrapError(std::move(res), "failed to remove value from memory storage");
  }
  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
