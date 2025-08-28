#include <stewkk/db/logic/filesystem/filesystem.hpp>

#include <fcntl.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::filesystem {

using result::MakeError;
using result::Result;
namespace fs = std::filesystem;

namespace {

static constexpr std::string_view kVersion = "v1";
static constexpr std::string_view kPathFormat = "{}/{}-{}.{}";

static constexpr std::string_view kDir = "/tmp/iu9-distributed-db";

}  // namespace

Result<std::ofstream> CreateBinaryFile(fs::path path) {
  try {
    std::ofstream res;
    res.exceptions(~std::ofstream::goodbit);
    res.open(path, std::ofstream::out | std::ofstream::binary);
    return res;
  } catch (const std::exception& ex) {
    return MakeError("failed to create file at {}: {}", path.string(), ex.what());
  }
}

Result<std::ifstream> OpenBinaryFile(fs::path path) {
  try {
    std::ifstream res;
    res.exceptions(~std::ofstream::goodbit);
    res.open(path, std::ifstream::in | std::ifstream::binary);
    return res;
  } catch (const std::exception& ex) {
    return MakeError("failed to open file at {}: {}", path.string(), ex.what());
  }
}

Result<std::int32_t> OpenBinaryFD(fs::path path) {
  auto fd = open(path.c_str(), O_RDONLY);
  if (fd == -1) {
    return MakeError("failed to open file at {}", path.string());
  }
  return Ok(fd);
}

fs::path GetPath(std::string_view extension) {
  return std::format(kPathFormat, kDir, kVersion,
                     boost::uuids::to_string(boost::uuids::random_generator()()), extension);
}

}  // namespace stewkk::db::logic::filesystem
