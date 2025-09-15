#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Remove(const boost::asio::yield_context& yield,
                                    models::dto::KeyDTO data) {
  auto [key, version] = std::move(data);
  if (!version.has_value()) {
    version = version_generator_.Generate();
    boost::asio::spawn(executor_, [&](boost::asio::yield_context yield) {
      // SendToReplicas(yield);
    });
  } else {
    version_generator_.UpdateMaxVersion(data.version.value());
  }

  auto err = storage_system_.Remove(yield, key, version.value());
  if (err.has_failure()) {
    return err;
  }

  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
