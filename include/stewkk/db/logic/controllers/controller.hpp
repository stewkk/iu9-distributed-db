#pragma once

#include <stewkk/db/logic/controllers/get.hpp>
#include <stewkk/db/logic/controllers/insert.hpp>
#include <stewkk/db/logic/controllers/remove.hpp>
#include <stewkk/db/logic/controllers/update.hpp>
#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::controllers {

class Controller : public GetController,
                   public InsertController,
                   public RemoveController,
                   public UpdateController {
public:
  virtual result::Result<> Insert(KwDTO data) override;
  virtual result::Result<> Update(KwDTO data) override;
  virtual result::Result<> Remove(KeyDTO data) override;
  virtual result::Result<ValueDTO> Get(KeyDTO data) override;

private:
};

}  // namespace stewkk::db::logic::controllers
