#pragma once

#include <cstdint>
#include <string>

namespace stewkk::db::models::storage {

struct KwPair {
  std::string key;
  std::string value;
  uint64_t version;

  auto operator<=>(const KwPair& other) const = default;
};

}  // namespace stewkk::db::models::storage
