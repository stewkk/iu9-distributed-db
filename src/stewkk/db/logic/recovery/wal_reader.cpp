#include <stewkk/db/logic/recovery/wal_reader.hpp>

#include <absl/log/log.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <wal.pb.h>

#include <stewkk/db/logic/filesystem/filesystem.hpp>

namespace stewkk::db::logic::recovery {

Result<std::pair<std::vector<Operation>, int64_t>> ReadWAL(fs::path path) {
  LOG(INFO) << std::format("loading WAL file: {}", path.string());
  auto f_opt = filesystem::OpenBinaryFD(path);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), "failed to open WAL file");
  }
  auto f = std::move(f_opt).assume_value();

  google::protobuf::io::FileInputStream stream(f);
  std::vector<Operation> result;

  int64_t last_valid = 0;
  for (;;) {
    last_valid = stream.ByteCount();
    wal::Entry entry;
    bool is_eof = false;
    bool is_ok = google::protobuf::util::ParseDelimitedFromZeroCopyStream(&entry, &stream, &is_eof);
    if (is_eof) {
      break;
    }
    if (!is_ok) {
      LOG(ERROR) << std::format("found bad entry in WAL file {} at {}", path.string(), last_valid);
      return result::Ok(std::make_pair(std::move(result), last_valid));
    }

    // NOTE: better to move into separate function
    if (entry.has_insert()) {
      result.push_back(Operation{
          OperationType::kInsert,
          entry.insert().key(),
          entry.insert().value(),
      });
    } else if (entry.has_remove()) {
      result.push_back(Operation{
          OperationType::kRemove,
          entry.remove().key(),
          std::nullopt,
      });
    }
  }

  return result::Ok(std::make_pair(std::move(result), last_valid));
}

std::string ToString(OperationType type) {
  switch (type) {
    case OperationType::kInsert:
      return "insert";
    case OperationType::kRemove:
      return "remove";
  }
}

Result<std::vector<fs::path>> SearchWALFiles() {
  std::vector<fs::path> result;
  result.reserve(2);
  auto dir = filesystem::GetDataDir();
  try {
    for (const auto& entry : std::filesystem::directory_iterator{dir}) {
      if (entry.path().extension() == ".wal") {
        result.push_back(entry.path());
      }
    }
  } catch (const std::exception& ex) {
    return result::MakeError("failed to search for .wal files: {}", ex.what());
  }
  return result;
}

}  // namespace stewkk::db::logic::recovery
