#include <stewkk/db/logic/storage/filesystem.hpp>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::storage {

using result::Error;
using result::Result;
namespace fs = std::filesystem;

Result<std::ofstream> CreateBinaryFile(fs::path path) {
  try {
    std::ofstream res;
    res.exceptions(~std::ofstream::goodbit);
    res.open(path, std::ofstream::out | std::ofstream::binary);
    return res;
  } catch (const std::exception& ex) {
    return Error("failed to create file at {}: {}", path.string(), ex.what());
  }
}

Result<std::ifstream> OpenBinaryFile(fs::path path) {
  try {
    std::ifstream res;
    res.exceptions(~std::ofstream::goodbit);
    res.open(path, std::ifstream::in | std::ifstream::binary);
    return res;
  } catch (const std::exception& ex) {
    return Error("failed to open file at {}: {}", path.string(), ex.what());
  }
}

}  // namespace stewkk::db::logic::storage
