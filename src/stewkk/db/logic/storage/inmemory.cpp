#include <stewkk/db/logic/storage/memstorage_impl.hpp>

namespace stewkk::db::logic::storage {

using result::MakeError;

namespace {

constexpr static std::string_view kNotFound{"key {} not found in storage"};

}  // namespace

MapStorage::MapStorage(Map&& other) : map_(std::move(other)) {}

Result<std::optional<std::string>> MapStorage::Get(std::string key) {
  std::optional<std::string> value;
  bool found = map_.visit(key, [&value](const auto& x) { value = x.second; });
  if (!found) {
    return MakeError<result::ErrorType::kNotFound>(kNotFound, key);
  }
  return value;
}

void MapStorage::Remove(std::string key) { map_.erase(std::move(key)); }

void MapStorage::Insert(KwPair data) {
  map_.insert_or_assign(std::move(data.key), std::move(data.value));
}

void MapStorage::Clear() { map_.clear(); }

size_t MapStorage::Size() const { return map_.size(); }

MapStorage::Map&& MapStorage::MoveUnderlying() { return std::move(map_); }

std::vector<StorageEntry> MapStorage::Collect() {
  auto readonly = ReadonlyMemoryStorage(std::move(*this));
  map_ = Map();
  return {readonly.begin(), readonly.end()};
}

ReadonlyMemoryStorage::ReadonlyMemoryStorage(MapStorage&& storage)
    : map_(std::move(storage).MoveUnderlying()) {}

ReadonlyMemoryStorage::Iterator ReadonlyMemoryStorage::begin() const {
  return Iterator{map_.cbegin()};
}
ReadonlyMemoryStorage::Iterator ReadonlyMemoryStorage::end() const { return Iterator{map_.cend()}; }

ReadonlyMemoryStorage::Iterator::value_type ReadonlyMemoryStorage::Iterator::operator*() const {
  return StorageEntry{map_iterator_->first, map_iterator_->second};
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
