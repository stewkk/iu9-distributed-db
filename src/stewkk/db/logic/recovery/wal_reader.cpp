#include <stewkk/db/logic/recovery/wal_reader.hpp>

#include <wal.pb.h>

#include <stewkk/db/logic/filesystem/filesystem.hpp>

namespace stewkk::db::logic::recovery {

Result<std::vector<Operation>> ReadWAL(fs::path path) {
  return result::Ok(std::vector<Operation>{});
}

}  // namespace stewkk::db::logic::recovery
