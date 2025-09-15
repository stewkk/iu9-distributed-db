#include <stewkk/db/logic/controllers/controller.hpp>

#include <absl/log/log.h>
#include <boost/asio/detached.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Remove(const boost::asio::yield_context& yield,
                                    models::dto::KeyDTO data) {
  auto [key, version] = std::move(data);
  if (!version.has_value()) {
    version = version_generator_.Generate();
    boost::asio::spawn(
        executor_,
        [&](boost::asio::yield_context yield) {
          auto err = replication_.SendRemoveToReplicas(yield, data);
          if (err.has_failure()) {
            LOG(ERROR) << "error replicating remove request: " << err.assume_error().What();
          }
        },
        boost::asio::detached);
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
