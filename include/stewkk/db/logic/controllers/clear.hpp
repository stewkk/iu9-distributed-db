#pragma once

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::controllers {

class ClearController {
public:
  virtual result::Result<> Clear() = 0;

  virtual ~ClearController() = default;
};

}  // namespace stewkk::db::logic::controllers
