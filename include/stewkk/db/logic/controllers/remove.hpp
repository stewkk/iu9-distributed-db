#pragma once

#include <stewkk/db/logic/controllers/dto.hpp>
#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::controllers {

class RemoveController {
public:
  virtual result::Result<> Remove(KeyDTO data) = 0;

  virtual ~RemoveController() = default;
};

}  // namespace stewkk::db::logic::controllers
