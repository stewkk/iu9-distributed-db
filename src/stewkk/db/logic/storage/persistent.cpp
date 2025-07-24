#include <stewkk/db/logic/storage/persistent.hpp>

#include <format>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace stewkk::db::logic::storage {

using result::Ok;

namespace {

static constexpr std::string_view kVersion = "v1";
static constexpr std::string_view kPathFormat = "{}/{}-{}";

static constexpr std::string_view kDir = "/tmp/iu9-distributed-db";

}  // namespace

fs::path GetPath() {
  return std::format(kPathFormat, kDir, kVersion,
                     boost::uuids::to_string(boost::uuids::random_generator()()));
}

PersistentStorage::PersistentStorage() {}

Result<KwPair> PersistentStorage::Get(std::string_view key) const { return Ok(KwPair{}); }

}  // namespace stewkk::db::logic::storage
