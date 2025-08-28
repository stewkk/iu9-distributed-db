#include <stewkk/db/logic/storage/memstorage_impl.hpp>

namespace stewkk::db::logic::storage {

using result::MakeError;
using result::Ok;

namespace {

constexpr static std::string_view kNotFound{"key {} not found in storage"};

}  // namespace

Result<KwPair> MapStorage::Get(std::string key) {
  std::string value;
  bool found = map_.visit(key, [&value](const auto& x) { value = x.second; });
  if (!found) {
    return MakeError<result::ErrorType::kNotFound>(kNotFound, key);
  }
  return KwPair{.key = std::move(key), .value = std::move(value)};
}

Result<> MapStorage::Remove(std::string key) {
  bool is_erased = map_.erase(std::move(key));
  if (!is_erased) {
    return MakeError(kNotFound, key);
  }
  return Ok();
}

void MapStorage::Insert(KwPair data) {
  map_.insert_or_assign(std::move(data.key), std::move(data.value));
}

Result<> MapStorage::Update(KwPair data) {
  bool found = map_.visit(data.key, [&data](auto& x) { x.second = std::move(data.value); });
  if (!found) {
    return MakeError("trying to update non-existing key {}", std::move(data.key));
  }
  return Ok();
}

void MapStorage::Clear() { map_.clear(); }

MapStorage::Map&& MapStorage::MoveUnderlying() { return std::move(map_); }

std::vector<KwPair> MapStorage::Collect() {
  auto readonly = ReadonlyMemoryStorage(std::move(*this));
  map_ = Map();
  return std::vector<KwPair>{readonly.begin(), readonly.end()};
}

ReadonlyMemoryStorage::ReadonlyMemoryStorage(MapStorage&& storage)
    : map_(std::move(storage).MoveUnderlying()) {}

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
