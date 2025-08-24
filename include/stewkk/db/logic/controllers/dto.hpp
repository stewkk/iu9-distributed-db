#pragma once

#include <string>

namespace stewkk::db::logic::controllers {

struct KwDTO {
  std::string key;
  std::string value;
};

struct KeyDTO {
  std::string key;
};

struct ValueDTO {
  std::string value;
};

}  // namespace stewkk::db::logic::controllers
