#include <stewkk/db/logic/storage/storage_system.hpp>

#include <absl/log/log.h>

namespace stewkk::db::logic::storage {

StorageSystem::StorageSystem(recovery::WALWriter& wal_writer,
                             storage::SwappableMemoryStorage& storage,
                             storage::PersistentStorageCollection& persistent_storage)
    : wal_writer_(wal_writer), storage_(storage), persistent_storage_(persistent_storage) {}

result::Result<> StorageSystem::Insert(const boost::asio::yield_context& yield,
                                       models::storage::KwPair kw_pair) {
  auto old_opt = Get(yield, kw_pair.key);
  if (old_opt.has_value()) {
    auto old = std::move(old_opt).assume_value();

    if (kw_pair.version < old.version) {
      LOG(INFO) << std::format("omitting insert of key={} with version={} older than current={}",
                               kw_pair.key, kw_pair.version, old.version);
      return result::Ok();
    }
  }

  auto res = wal_writer_.Insert(yield, kw_pair);
  if (res.has_failure()) {
    return res;
  }
  storage_.Insert(std::move(kw_pair));
  return result::Ok();
}

result::Result<> StorageSystem::Remove(const boost::asio::yield_context& yield, std::string key,
                                       uint64_t version) {
  auto old_opt = Get(yield, key);
  if (old_opt.has_value()) {
    auto old = std::move(old_opt).assume_value();

    if (version < old.version) {
      LOG(INFO) << std::format("omitting remove of key={} with version={} older than current={}",
                               key, version, old.version);
      return result::Ok();
    }
  }

  auto res = wal_writer_.Remove(yield, key, version);
  if (res.has_failure()) {
    return res;
  }
  storage_.Remove(std::move(key), std::move(version));
  return result::Ok();
}

result::Result<models::dto::ValueDTO> StorageSystem::Get(const boost::asio::yield_context& yield,
                                                         std::string key) {
  auto mem_opt = storage_.Get(key);
  if (mem_opt.has_value()) {
    auto res = std::move(mem_opt).assume_value();
    if (res.value.has_value()) {
      return result::Ok(models::dto::ValueDTO{res.value.value()});
    }
    return result::MakeError<result::ErrorType::kNotFound>("key {} not found", key);
  }
  auto persistent_opt = persistent_storage_.Get(key, yield);
  if (persistent_opt.has_value()) {
    auto res = persistent_opt.assume_value();
    if (res.value.has_value()) {
      return result::Ok(models::dto::ValueDTO{res.value.value(), res.version});
    }
  }
  return result::MakeError<result::ErrorType::kNotFound>("key {} not found", std::move(key));
}

}  // namespace stewkk::db::logic::storage
