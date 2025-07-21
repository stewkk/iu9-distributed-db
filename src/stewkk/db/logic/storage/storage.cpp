#include <stewkk/db/logic/storage/storage.hpp>

namespace stewkk::db::logic::storage {

using result::Error;
using result::Ok;

Result<KwPair> MapStorage::Get(std::string key) {
  auto it = map_.find(key);
  if (it == map_.end()) {
    return Error("key {} not found in storage", key);
  }
  return KwPair{.key = key, .value = it->second};
}

Result<> MapStorage::Remove(std::string key) { return Ok({}); }

void MapStorage::Insert(KwPair data) { map_.emplace(std::move(data.key), std::move(data.value)); }

Result<> MapStorage::Update(KwPair data) { return Ok({}); }

}  // namespace stewkk::db::logic::storage
