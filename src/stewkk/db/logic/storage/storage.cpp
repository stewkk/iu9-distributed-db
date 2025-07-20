#include <stewkk/db/logic/storage/storage.hpp>

#include <format>

namespace stewkk::db::logic::storage {

std::exception_ptr Error(const std::exception& e) { return std::make_exception_ptr(e); }

Result<KwPair> MapStorage::Get(std::string key) {
  auto it = map_.find(key);
  if (it == map_.end()) {
    return Error(std::runtime_error{std::format("key {} not found in storage", key)});
  }
  return KwPair{.key = key, .value = it->second};
}

Result<> MapStorage::Remove(std::string key) { return Ok({}); }

void MapStorage::Insert(KwPair data) { map_.emplace(std::move(data.key), std::move(data.value)); }

Result<> MapStorage::Update(KwPair data) { return Ok({}); }

}  // namespace stewkk::db::logic::storage
