#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<ValueDTO> Controller::Get(KeyDTO data) {
  auto [key] = data;
  auto res = storage_.Get(std::move(key));
  if (res.has_failure()) {
    return result::WrapError(std::move(res), "failed to get value from memory storage");
  }
  return result::Ok(ValueDTO{res.assume_value().value});
}

}  // namespace stewkk::db::logic::controllers
