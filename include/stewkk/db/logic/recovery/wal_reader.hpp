#pragma once

#include <filesystem>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::recovery {

using result::Result;
namespace fs = std::filesystem;

enum class OperationType { kInsert, kUpdate, kRemove };

struct Operation {
  OperationType type;
  std::string key;
  std::optional<std::string> value;

  bool operator<=>(const Operation& other) const = default;
};

Result<std::vector<Operation>> ReadWAL(fs::path path);

}  // namespace stewkk::db::logic::recovery
