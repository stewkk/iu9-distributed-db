#pragma once

#include <filesystem>
#include <ranges>
#include <vector>

#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <stewkk/db/logic/filesystem/filesystem.hpp>
#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/collectable_memstorage.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using models::storage::KwPair;
using result::Result;
namespace fs = std::filesystem;

class PersistentStorage {
public:
  PersistentStorage(fs::path path, std::vector<uint64_t> index);
  Result<StorageEntry> Get(std::string_view key);
  fs::path Path() const;

private:
  Result<StorageEntry> ReadEntryAt(uint64_t offset);

private:
  fs::path path_;

  std::vector<uint64_t> index_;
};

Result<PersistentStorage> NewPersistentStorage(std::vector<StorageEntry> data);

Result<PersistentStorage> NewPersistentStorage(const std::ranges::input_range auto& provider) {
  std::vector<StorageEntry> data{provider.begin(), provider.end()};
  return NewPersistentStorage(std::move(data));
}

Result<PersistentStorage> LoadPersistentStorage(fs::path path);

}  // namespace stewkk::db::logic::storage
