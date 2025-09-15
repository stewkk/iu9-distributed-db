#include <stewkk/db/logic/controllers/controller.hpp>

#include <absl/log/log.h>
#include <boost/asio/detached.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Insert(const boost::asio::yield_context& yield,
                                    models::dto::KwDTO data) {
  if (!data.version.has_value()) {
    data.version = version_generator_.Generate();
    boost::asio::spawn(
        executor_,
        [&](boost::asio::yield_context yield) {
          auto err = replication_.SendInsertToReplicas(yield, data);
          if (err.has_failure()) {
            LOG(ERROR) << "error replicating insert request: " << err.assume_error().What();
          }
        },
        boost::asio::detached);
  } else {
    version_generator_.UpdateMaxVersion(data.version.value());
  }

  auto [key, value, version] = std::move(data);
  models::storage::KwPair kw_pair{std::move(key), std::move(value), std::move(version).value()};
  auto err = storage_system_.Insert(yield, std::move(kw_pair));
  if (err.has_failure()) {
    return err;
  }

  TriggerSwapToPersistentStorage();
  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
