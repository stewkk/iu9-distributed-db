#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<models::dto::ValueDTO> Controller::Get(const boost::asio::yield_context& yield,
                                                      models::dto::GetRequestDTO data) {
  auto [key] = data;
  return storage_system_.Get(yield, std::move(key));
}

}  // namespace stewkk::db::logic::controllers
