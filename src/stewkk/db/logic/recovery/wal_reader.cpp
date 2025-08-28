#include <stewkk/db/logic/recovery/wal_reader.hpp>

#include <absl/log/log.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <wal.pb.h>

#include <stewkk/db/logic/filesystem/filesystem.hpp>

namespace stewkk::db::logic::recovery {

Result<std::vector<Operation>> ReadWAL(fs::path path) {
  LOG(INFO) << std::format("loading WAL file: {}", path.string());
  auto f_opt = filesystem::OpenBinaryFD(path);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), "failed to open WAL file");
  }
  auto f = std::move(f_opt).assume_value();

  google::protobuf::io::FileInputStream stream(f);
  std::vector<Operation> result;

  for (;;) {
    wal::Entry entry;
    bool is_eof = false;
    bool is_ok = google::protobuf::util::ParseDelimitedFromZeroCopyStream(&entry, &stream, &is_eof);
    if (is_eof) {
      break;
    }
    if (!is_ok) {
      // TODO: maybe just log error and break?
      return result::MakeError("failed to parse WAL file: {}", path.string());
    }

    // NOTE: better to move into separate function
    if (entry.has_insert()) {
      result.push_back(Operation{
          OperationType::kInsert,
          entry.insert().key(),
          entry.insert().value(),
      });
    } else if (entry.has_update()) {
      result.push_back(Operation{
          OperationType::kUpdate,
          entry.update().key(),
          entry.update().value(),
      });
    } else if (entry.has_remove()) {
      result.push_back(Operation{
          OperationType::kRemove,
          entry.remove().key(),
          std::nullopt,
      });
    }
  }

  return result::Ok(std::move(result));
}

std::string ToString(OperationType type) {
  switch (type) {
    case OperationType::kInsert:
      return "insert";
    case OperationType::kUpdate:
      return "update";
    case OperationType::kRemove:
      return "remove";
  }
}

}  // namespace stewkk::db::logic::recovery
