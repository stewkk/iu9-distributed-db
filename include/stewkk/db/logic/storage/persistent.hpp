#pragma once

#include <filesystem>
#include <ranges>
#include <vector>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/filesystem.hpp>
#include <stewkk/db/logic/storage/persistent_data.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using models::storage::KwPair;
using result::Result;
namespace fs = std::filesystem;

using Index = std::vector<PersistentKwPair>;

class PersistentStorage {
public:
  PersistentStorage(fs::path path, std::ifstream stream, Metadata metadata, Index index);
  Result<KwPair> Get(std::string_view key);

private:
  std::string ReadString(PersistentString persistent_str);

private:
  fs::path path_;
  std::ifstream f_;

  std::uint64_t data_offset_;

  Metadata metadata_;
  Index index_;
};

class IndexBuilder {
public:
  explicit IndexBuilder();
  void AddRecord(const KwPair& pair);
  Index GetIndex() const;

private:
  PersistentString GetPersistentString(const std::string& str);

private:
  std::uint64_t offset_;
  Index index_;
};

Result<PersistentStorage> NewPersistentStorage(std::vector<KwPair> data);

Result<PersistentStorage> NewPersistentStorage(const std::ranges::input_range auto& provider) {
  std::vector<KwPair> data{provider.begin(), provider.end()};
  return NewPersistentStorage(std::move(data));
}

}  // namespace stewkk::db::logic::storage
