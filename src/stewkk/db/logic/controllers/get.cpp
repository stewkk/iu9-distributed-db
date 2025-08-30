#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<ValueDTO> Controller::Get(KeyDTO data) {
  auto [key] = data;
  auto res = storage_.Get(key);
  if (res.has_value()) {
    return result::Ok(ValueDTO{res.assume_value().value});
  }
  res = persistent_storage_.Get(key);
  if (res.has_value()) {
    return result::Ok(ValueDTO{res.assume_value().value});
  }
  return result::MakeError<result::ErrorType::kNotFound>("key {} not found", std::move(key));
}

}  // namespace stewkk::db::logic::controllers
