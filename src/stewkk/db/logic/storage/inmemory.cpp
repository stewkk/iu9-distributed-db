#include <stewkk/db/logic/storage/inmemory.hpp>

namespace stewkk::db::logic::storage {

using result::Error;
using result::Ok;

namespace {

constexpr static std::string_view kNotFound{"key {} not found in storage"};

}  // namespace

Result<KwPair> MemoryStorage::Get(std::string key) {
  auto it = map_.find(key);
  if (it == map_.end()) {
    return Error(kNotFound, key);
  }
  return KwPair{.key = key, .value = it->second};
}

Result<> MemoryStorage::Remove(std::string key) {
  auto it = map_.find(key);
  if (it == map_.end()) {
    return Error(kNotFound, key);
  }
  map_.erase(it);
  return Ok();
}

void MemoryStorage::Insert(KwPair data) {
  map_.insert_or_assign(std::move(data.key), std::move(data.value));
}

Result<> MemoryStorage::Update(KwPair data) {
  auto it = map_.find(data.key);
  if (it == map_.end()) {
    return Error("trying to update non-existing key {}", data.key);
  }
  it->second = std::move(data.value);
  return Ok();
}

MemoryStorage::Iterator MemoryStorage::begin() const { return Iterator{map_.cbegin()}; }
MemoryStorage::Iterator MemoryStorage::end() const { return Iterator{map_.cend()}; }

MemoryStorage::Iterator::value_type MemoryStorage::Iterator::operator*() const {
  return KwPair{.key = map_iterator_->first, .value = map_iterator_->second};
}

MemoryStorage::Iterator& MemoryStorage::Iterator::operator++() {
  ++map_iterator_;
  return *this;
}

MemoryStorage::Iterator MemoryStorage::Iterator::operator++(int) {
  auto tmp = *this;
  ++map_iterator_;
  return tmp;
}

}  // namespace stewkk::db::logic::storage
