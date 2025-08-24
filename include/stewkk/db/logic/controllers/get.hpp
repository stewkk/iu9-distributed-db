#pragma once

#include <stewkk/db/logic/controllers/dto.hpp>
#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::controllers {

class GetController {
public:
  virtual result::Result<ValueDTO> Get(KeyDTO data) = 0;

  virtual ~GetController() = default;
};

}  // namespace stewkk::db::logic::controllers
