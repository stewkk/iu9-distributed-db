#pragma once

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/persistent.hpp>

namespace stewkk::db::logic::storage {

class PersistentStorageCollection {
public:
  PersistentStorageCollection();

  Result<> Add(PersistentStorage&& storage);
  Result<StorageEntry> Get(std::string_view key);

private:
  std::vector<PersistentStorage> collection_;
};

}  // namespace stewkk::db::logic::storage
