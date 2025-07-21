#include <stewkk/db/logic/storage/storage.hpp>

namespace stewkk::db::logic::storage {

using result::Error;
using result::Ok;

namespace {

constexpr static std::string_view kNotFound{"key {} not found in storage"};

}  // namespace

Result<KwPair> MapStorage::Get(std::string key) {
  auto it = map_.find(key);
  if (it == map_.end()) {
    return Error(kNotFound, key);
  }
  return KwPair{.key = key, .value = it->second};
}

Result<> MapStorage::Remove(std::string key) {
  auto it = map_.find(key);
  if (it == map_.end()) {
    return Error(kNotFound, key);
  }
  map_.erase(it);
  return Ok();
}

void MapStorage::Insert(KwPair data) {
  map_.insert_or_assign(std::move(data.key), std::move(data.value));
}

Result<> MapStorage::Update(KwPair data) {
  auto it = map_.find(data.key);
  if (it == map_.end()) {
    return Error("trying to update non-existing key {}", data.key);
  }
  it->second = std::move(data.value);
  return Ok();
}

}  // namespace stewkk::db::logic::storage
