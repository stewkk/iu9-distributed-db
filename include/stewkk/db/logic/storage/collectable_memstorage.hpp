#pragma once

#include <vector>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using result::Result;

// TODO: вынести в отдельный файл
struct StorageEntry {
  std::string key;
  std::optional<std::string> value;

  auto operator<=>(const StorageEntry& other) const = default;
};

class CollectableStorage {
public:
  virtual std::vector<StorageEntry> Collect() = 0;

  virtual ~CollectableStorage() = default;
};

}  // namespace stewkk::db::logic::storage
