#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::result {

std::string What(const Error& error) { return error.What(); }

}  // namespace stewkk::db::logic::result
