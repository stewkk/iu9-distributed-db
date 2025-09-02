#pragma once

#include <filesystem>
#include <fstream>

#include <wal.pb.h>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::recovery {

using models::storage::KwPair;
using result::Result;
namespace fs = std::filesystem;

class WALWriterImpl {
public:
  WALWriterImpl(fs::path path, std::ofstream&& stream);

  Result<> WriteEntry(wal::Entry entry);
  fs::path GetPath() const;

private:
  fs::path path_;
  std::ofstream f_;
};

Result<WALWriterImpl> NewWALWriter();

Result<WALWriterImpl> LoadWALWriter(fs::path path, int64_t seek);

}  // namespace stewkk::db::logic::recovery
