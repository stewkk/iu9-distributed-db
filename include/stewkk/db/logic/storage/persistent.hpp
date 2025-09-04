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
  using Index = std::vector<uint64_t>;

  PersistentStorage(fs::path path, int fd, Index index);
  ~PersistentStorage();
  PersistentStorage(const PersistentStorage& other) = delete;
  PersistentStorage(PersistentStorage&& other);
  PersistentStorage& operator=(const PersistentStorage& other) = delete;
  PersistentStorage& operator=(PersistentStorage&& other);

  Result<StorageEntry> Get(std::string_view key);
  Result<StorageEntry> ReadEntryAt(uint64_t offset) const;
  fs::path Path() const;

  struct Iterator {
    using difference_type = std::ptrdiff_t;
    using value_type = Result<StorageEntry>;

    value_type operator*() const;

    Iterator& operator++();
    Iterator operator++(int);

    bool operator==(const Iterator& other) const = default;

    const PersistentStorage* storage;
    PersistentStorage::Index::const_iterator index_iterator;
  };
  static_assert(std::input_iterator<Iterator>);

  Iterator begin() const;
  Iterator end() const;

private:
  fs::path path_;
  int fd_;

  Index index_;
};

Result<PersistentStorage> NewPersistentStorage(std::vector<StorageEntry> data);

Result<PersistentStorage> NewPersistentStorage(const std::ranges::input_range auto& provider) {
  std::vector<StorageEntry> data{provider.begin(), provider.end()};
  return NewPersistentStorage(std::move(data));
}

Result<PersistentStorage> LoadPersistentStorage(fs::path path);

}  // namespace stewkk::db::logic::storage
