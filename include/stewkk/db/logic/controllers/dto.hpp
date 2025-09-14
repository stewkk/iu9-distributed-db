#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace stewkk::db::logic::controllers {

struct KwDTO {
  std::string key;
  std::string value;
  std::optional<uint64_t> version;
};

struct KeyDTO {
  std::string key;
  std::optional<uint64_t> version;
};

struct ValueDTO {
  std::string value;
  uint64_t version;
};

struct GetRequestDTO {
  std::string key;
};

}  // namespace stewkk::db::logic::controllers
