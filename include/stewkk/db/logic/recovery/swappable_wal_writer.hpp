#pragma once

#include <functional>

#include <stewkk/db/logic/recovery/wal_writer.hpp>

namespace stewkk::db::logic::recovery {

using RemoveWalCallback = std::function<void()>;

class SwappableWalWriter {
public:
  virtual Result<RemoveWalCallback> Swap() = 0;

  virtual ~SwappableWalWriter() = default;
};

}  // namespace stewkk::db::logic::recovery
