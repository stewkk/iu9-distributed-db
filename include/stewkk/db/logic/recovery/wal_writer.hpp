#pragma once

#include <filesystem>
#include <fstream>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::recovery {

using models::storage::KwPair;
using result::Result;
namespace fs = std::filesystem;

class WALWriter {
public:
  WALWriter(fs::path path, std::ofstream&& stream);

  Result<> Remove(std::string key);
  Result<> Insert(KwPair data);
  Result<> Update(KwPair data);

private:
  void Log();

private:
  fs::path path_;
  std::ofstream f_;
};

Result<WALWriter> NewWALWriter();

}  // namespace stewkk::db::logic::recovery
