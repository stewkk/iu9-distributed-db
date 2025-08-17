#include <stewkk/db/logic/storage/inmemory.hpp>

namespace stewkk::db::logic::storage {

using result::Error;
using result::Ok;

namespace {

constexpr static std::string_view kNotFound{"key {} not found in storage"};

}  // namespace

Result<KwPair> MemoryStorage::Get(std::string key) {
  std::string value;
  bool found = map_.visit(key, [&value](const auto& x) { value = x.second; });
  if (!found) {
    return Error(kNotFound, key);
  }
  return KwPair{.key = std::move(key), .value = std::move(value)};
}

Result<> MemoryStorage::Remove(std::string key) {
  bool is_erased = map_.erase(std::move(key));
  if (!is_erased) {
    return Error(kNotFound, key);
  }
  return Ok();
}

void MemoryStorage::Insert(KwPair data) {
  map_.insert_or_assign(std::move(data.key), std::move(data.value));
}

Result<> MemoryStorage::Update(KwPair data) {
  bool found = map_.visit(data.key, [&data](auto& x) { x.second = std::move(data.value); });
  if (!found) {
    return Error("trying to update non-existing key {}", std::move(data.key));
  }
  return Ok();
}

MemoryStorage::Map&& MemoryStorage::GetUnderlying() && { return std::move(map_); }

ReadonlyMemoryStorage::ReadonlyMemoryStorage(MemoryStorage&& storage)
    : map_(std::move(storage).GetUnderlying()) {}

ReadonlyMemoryStorage::Iterator ReadonlyMemoryStorage::begin() const {
  return Iterator{map_.cbegin()};
}
ReadonlyMemoryStorage::Iterator ReadonlyMemoryStorage::end() const { return Iterator{map_.cend()}; }

ReadonlyMemoryStorage::Iterator::value_type ReadonlyMemoryStorage::Iterator::operator*() const {
  return KwPair{.key = map_iterator_->first, .value = map_iterator_->second};
}

ReadonlyMemoryStorage::Iterator& ReadonlyMemoryStorage::Iterator::operator++() {
  ++map_iterator_;
  return *this;
}

ReadonlyMemoryStorage::Iterator ReadonlyMemoryStorage::Iterator::operator++(int) {
  auto tmp = *this;
  ++map_iterator_;
  return tmp;
}

}  // namespace stewkk::db::logic::storage
