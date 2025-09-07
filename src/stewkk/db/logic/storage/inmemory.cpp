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

void MapStorage::Remove(std::string key) { map_.insert_or_assign(std::move(key), std::nullopt); }

void MapStorage::Insert(KwPair data) {
  map_.insert_or_assign(std::move(data.key), std::move(data.value));
}

size_t MapStorage::Size() { return map_.size(); }

const MapStorage::Map& MapStorage::GetUnderlying() const { return map_; }

std::vector<StorageEntry> ToEntries(
    boost::unordered_flat_map<std::string, std::optional<std::string>> map) {
  std::vector<StorageEntry> res;
  res.reserve(map.size());
  for (const auto& entry : map) {
    res.emplace_back(entry.first, entry.second);
  }
  return res;
}

}  // namespace stewkk::db::logic::storage
