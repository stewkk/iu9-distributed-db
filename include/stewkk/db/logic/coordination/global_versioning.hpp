#pragma once

#include <atomic>
#include <cstdint>

namespace stewkk::db::logic::coordination {

class VersionNumberGenerator {
public:
  explicit VersionNumberGenerator(uint64_t start);
  uint64_t Generate();

private:
  std::atomic<uint64_t> current_;
};

}  // namespace stewkk::db::logic::coordination
