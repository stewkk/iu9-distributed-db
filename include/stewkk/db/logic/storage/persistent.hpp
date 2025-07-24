#pragma once

#include <filesystem>
#include <ranges>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/filesystem.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using models::storage::KwPair;
using result::Result;
namespace fs = std::filesystem;

fs::path GetPath();

class PersistentStorage {
public:
  Result<KwPair> Get(std::string_view key) const;

private:
  friend Result<PersistentStorage> NewPersistentStorage(
      const std::ranges::input_range auto& provider);

  PersistentStorage();
};

Result<PersistentStorage> NewPersistentStorage(const std::ranges::input_range auto& provider) {
  auto path = GetPath();
  auto f = CreateFile(path);
  if (f.has_failure()) {
    return result::WrapError(std::move(f), "failed to create persistent storage");
  }
  return PersistentStorage();
}

}  // namespace stewkk::db::logic::storage
