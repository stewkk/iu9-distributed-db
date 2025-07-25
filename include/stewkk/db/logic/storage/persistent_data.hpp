#pragma once

#include <cstdint>

namespace stewkk::db::logic::storage {

struct Metadata {
  std::uint64_t map_size;
};

struct PersistentString {
  std::uint64_t index;
  std::uint64_t size;
};

struct PersistentKwPair {
  PersistentString key;
  PersistentString value;
};

}  // namespace stewkk::db::logic::storage
