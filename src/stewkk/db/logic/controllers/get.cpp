#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<models::dto::ValueDTO> Controller::Get(const boost::asio::yield_context& yield,
                                                      models::dto::GetRequestDTO data) {
  auto [key] = data;
  auto got_replicas = replication_.GetNewestFromReplicas(yield, data);
  auto got_local = storage_system_.Get(yield, std::move(key));
  if (!got_replicas.has_value()) {
    return got_local;
  }
  if (!got_local.has_value()
      || got_local.assume_value().version < got_replicas.assume_value().version) {
    auto value = got_replicas.assume_value();
    // TODO: make GetNewestFromReplicas return optional value instead of not found error
    auto _ = storage_system_.Insert(yield,
                                    models::storage::KwPair{data.key, value.value, value.version});
    return value;
  }
  return got_local;
}

}  // namespace stewkk::db::logic::controllers
