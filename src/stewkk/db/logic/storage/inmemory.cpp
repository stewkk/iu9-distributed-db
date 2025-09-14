#include <stewkk/db/logic/storage/memstorage_impl.hpp>

namespace stewkk::db::logic::storage {

using result::MakeError;

namespace {

constexpr static std::string_view kNotFound{"key {} not found in storage"};

}  // namespace

MapStorage::MapStorage(Map&& other) : map_(std::move(other)) {}

Result<Value> MapStorage::Get(std::string key) {
  Value value;
  bool found = map_.visit(key, [&value](const auto& x) { value = x.second; });
  if (!found) {
    return MakeError<result::ErrorType::kNotFound>(kNotFound, key);
  }
  return value;
}

void MapStorage::Remove(std::string key, uint64_t version) {
  map_.insert_or_assign(std::move(key), Value{std::nullopt, version});
}

void MapStorage::Insert(KwPair data) {
  map_.insert_or_assign(std::move(data.key), Value{std::move(data.value), std::move(data.version)});
}

size_t MapStorage::Size() { return map_.size(); }

const MapStorage::Map& MapStorage::GetUnderlying() const { return map_; }

std::vector<StorageEntry> ToEntries(boost::unordered_flat_map<std::string, Value> map) {
  std::vector<StorageEntry> res;
  res.reserve(map.size());
  for (const auto& entry : map) {
    res.emplace_back(entry.first, entry.second.value, entry.second.version);
  }
  return res;
}

}  // namespace stewkk::db::logic::storage
