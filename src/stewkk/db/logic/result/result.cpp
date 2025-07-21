#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::result {

std::exception_ptr Error(const std::exception& e) { return std::make_exception_ptr(e); }

}  // namespace stewkk::db::logic::result
