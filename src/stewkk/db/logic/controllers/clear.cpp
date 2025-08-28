#include <stewkk/db/logic/controllers/controller.hpp>

namespace stewkk::db::logic::controllers {

result::Result<> Controller::Clear() {
  storage_.Clear();
  return result::Ok();
}

}  // namespace stewkk::db::logic::controllers
