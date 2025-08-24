#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<ValueDTO> Controller::Get(KeyDTO data) { return result::Ok(ValueDTO{"blabla"}); }

}  // namespace stewkk::db::logic::controllers
