#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<ValueDTO> Controller::Get(const boost::asio::yield_context& yield,
                                         GetRequestDTO data) {
  auto [key] = data;
  auto mem_opt = storage_.Get(key);
  if (mem_opt.has_value()) {
    auto res = std::move(mem_opt).assume_value();
    if (res.value.has_value()) {
      return result::Ok(ValueDTO{res.value.value()});
    }
    return result::MakeError<result::ErrorType::kNotFound>("key {} not found", key);
  }
  auto persistent_opt = persistent_storage_.Get(key, yield);
  if (persistent_opt.has_value()) {
    auto res = persistent_opt.assume_value();
    if (res.value.has_value()) {
      return result::Ok(ValueDTO{res.value.value(), res.version});
    }
  }
  return result::MakeError<result::ErrorType::kNotFound>("key {} not found", std::move(key));
}

}  // namespace stewkk::db::logic::controllers
