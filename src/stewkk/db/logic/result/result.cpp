#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::result {

std::string What(std::exception_ptr ex_ptr) {
  if (!ex_ptr) {
    return "nullptr exception";
  }
  try {
    std::rethrow_exception(ex_ptr);
  } catch (const std::exception& ex) {
    return ex.what();
  }
}

}  // namespace stewkk::db::logic::result
