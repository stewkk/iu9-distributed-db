#include <stewkk/db/logic/recovery/wal_writer.hpp>

#include <absl/log/log.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <wal.pb.h>

#include <stewkk/db/logic/storage/filesystem.hpp>

namespace stewkk::db::logic::recovery {

namespace {

static constexpr std::string_view kVersion = "v1";
static constexpr std::string_view kPathFormat = "{}/{}-{}.wal";

static constexpr std::string_view kDir = "/tmp/iu9-distributed-db";

static constexpr std::string_view kFailedToCreate = "failed to create WAL";

fs::path GetPath() {
  return std::format(kPathFormat, kDir, kVersion,
                     boost::uuids::to_string(boost::uuids::random_generator()()));
}

}  // namespace

WALWriter::WALWriter(fs::path path, std::ofstream&& stream)
    : path_(std::move(path)), f_(std::move(stream)) {}

Result<> WALWriter::Remove(std::string key) {
  wal::Entry entry;
  auto* remove_op = entry.mutable_remove();
  remove_op->set_key(std::move(key));
  bool is_ok = entry.SerializeToOstream(&f_);
  if (!is_ok) {
    return result::Error("failed to write log entry");
  }
  return result::Ok();
}

Result<> WALWriter::Insert(KwPair data) {
  wal::Entry entry;
  auto* insert_op = entry.mutable_insert();
  insert_op->set_key(std::move(data).key);
  insert_op->set_value(std::move(data).value);
  bool is_ok = entry.SerializeToOstream(&f_);
  if (!is_ok) {
    return result::Error("failed to write log entry");
  }
  return result::Ok();
}

Result<> WALWriter::Update(KwPair data) {
  wal::Entry entry;
  auto* update_op = entry.mutable_update();
  update_op->set_key(std::move(data).key);
  update_op->set_value(std::move(data).value);
  bool is_ok = entry.SerializeToOstream(&f_);
  if (!is_ok) {
    return result::Error("failed to write log entry");
  }
  return result::Ok();
}

Result<WALWriter> NewWALWriter() {
  auto path = GetPath();
  LOG(INFO) << std::format("creating new WAL at {}", path.string());
  auto f_opt = storage::CreateBinaryFile(path);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), kFailedToCreate);
  }
  auto f = std::move(f_opt).assume_value();

  return WALWriter(std::move(path), std::move(f));
}

}  // namespace stewkk::db::logic::recovery
