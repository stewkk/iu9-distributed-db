#include <stewkk/db/logic/recovery/wal_writer_impl.hpp>

#include <absl/log/log.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <stewkk/db/logic/filesystem/filesystem.hpp>

namespace stewkk::db::logic::recovery {

namespace {

static constexpr std::string_view kFailedToCreate = "failed to create WAL";

}  // namespace

WALWriterImpl::WALWriterImpl(fs::path path, std::ofstream&& stream)
    : path_(std::move(path)), f_(std::move(stream)) {}

Result<> WALWriterImpl::WriteEntry(wal::Entry entry) {
  bool is_ok = google::protobuf::util::SerializeDelimitedToOstream(entry, &f_);
  f_.flush();

  if (!is_ok) {
    return result::MakeError("failed to write log entry");
  }
  return result::Ok();
}

Result<WALWriterImpl> NewWALWriter() {
  auto extension = "wal";
  auto path = filesystem::GetPath(extension);
  LOG(INFO) << std::format("creating new WAL at {}", path.string());
  auto f_opt = filesystem::CreateBinaryFile(path);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), kFailedToCreate);
  }
  auto f = std::move(f_opt).assume_value();

  return WALWriterImpl(std::move(path), std::move(f));
}

Result<WALWriterImpl> LoadWALWriter(fs::path path, int64_t seek) {
  auto f_opt = filesystem::CreateBinaryFile(path, std::ofstream::in);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), kFailedToCreate);
  }
  auto f = std::move(f_opt).assume_value();

  f.seekp(seek);

  return WALWriterImpl(std::move(path), std::move(f));
}

fs::path WALWriterImpl::GetPath() const { return path_; }

}  // namespace stewkk::db::logic::recovery
