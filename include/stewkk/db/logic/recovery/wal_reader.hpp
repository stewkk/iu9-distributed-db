#pragma once

#include <filesystem>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::recovery {

using result::Result;
namespace fs = std::filesystem;

enum class OperationType { kInsert, kUpdate, kRemove };

std::string ToString(OperationType type);

struct Operation {
  OperationType type;
  std::string key;
  std::optional<std::string> value;

  bool operator<=>(const Operation& other) const = default;
};

Result<std::pair<std::vector<Operation>, int64_t>> ReadWAL(fs::path path);

Result<std::vector<fs::path>> SearchWALFiles();

}  // namespace stewkk::db::logic::recovery
