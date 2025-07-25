#pragma once

#include <filesystem>
#include <fstream>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::storage {

using result::Result;
namespace fs = std::filesystem;

Result<std::ofstream> CreateBinaryFile(fs::path path);

Result<std::ifstream> OpenBinaryFile(fs::path path);

}  // namespace stewkk::db::logic::storage
