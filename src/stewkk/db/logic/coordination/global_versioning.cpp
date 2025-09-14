#include <stewkk/db/logic/coordination/global_versioning.hpp>

namespace stewkk::db::logic::coordination {

VersionNumberGenerator::VersionNumberGenerator(uint64_t start) : current_(start) {}

uint64_t VersionNumberGenerator::Generate() { return current_.fetch_add(1); }

VersionNumberGenerator::VersionNumberGenerator(VersionNumberGenerator&& other)
    : current_(other.current_.exchange(0)) {}

VersionNumberGenerator& VersionNumberGenerator::operator=(VersionNumberGenerator&& other) {
  current_ = other.current_.exchange(0);
  return *this;
}

}  // namespace stewkk::db::logic::coordination
