#pragma once

#include <string>

namespace stewkk::db::models::storage {

struct KwPair {
  std::string key;
  std::string value;

  auto operator<=>(const KwPair& other) const = default;
};

}  // namespace stewkk::db::models::storage
