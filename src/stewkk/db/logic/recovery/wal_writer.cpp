#include <stewkk/db/logic/recovery/wal_writer_impl.hpp>

#include <absl/log/log.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <stewkk/db/logic/filesystem/filesystem.hpp>

namespace stewkk::db::logic::recovery {

namespace {

static constexpr std::string_view kFailedToCreate = "failed to create WAL";

}  // namespace

WALWriterImpl::WALWriterImpl(boost::asio::executor executor, fs::path path, std::ofstream&& stream)
    : path_(std::move(path)),
      f_(std::move(stream)),
      strand_(boost::asio::make_strand(executor)),
      executor_(std::move(executor)) {}

Result<> WALWriterImpl::WriteEntry(const boost::asio::yield_context& yield, wal::Entry entry) {
  boost::asio::post(boost::asio::bind_executor(strand_, yield));
  bool is_ok = google::protobuf::util::SerializeDelimitedToOstream(entry, &f_);
  f_.flush();
  boost::asio::post(boost::asio::bind_executor(executor_, yield));

  if (!is_ok) {
    return result::MakeError("failed to write log entry");
  }
  return result::Ok();
}

Result<> WALWriterImpl::Remove(const boost::asio::yield_context& yield, std::string key) {
  wal::Entry entry;
  auto* remove_op = entry.mutable_remove();
  remove_op->set_key(std::move(key));

  return WriteEntry(std::move(yield), std::move(entry));
}

Result<> WALWriterImpl::Insert(const boost::asio::yield_context& yield, KwPair data) {
  wal::Entry entry;
  auto* insert_op = entry.mutable_insert();
  insert_op->set_key(std::move(data).key);
  insert_op->set_value(std::move(data).value);

  return WriteEntry(yield, std::move(entry));
}

Result<WALWriterImpl> NewWALWriter(boost::asio::executor executor) {
  auto extension = "wal";
  auto path = filesystem::GetPath(extension);
  LOG(INFO) << std::format("creating new WAL at {}", path.string());
  auto f_opt = filesystem::CreateBinaryFile(path);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), kFailedToCreate);
  }
  auto f = std::move(f_opt).assume_value();

  return WALWriterImpl(std::move(executor), std::move(path), std::move(f));
}

Result<WALWriterImpl> LoadWALWriter(boost::asio::executor executor, fs::path path, int64_t seek) {
  auto f_opt = filesystem::CreateBinaryFile(path, std::ofstream::in);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), kFailedToCreate);
  }
  auto f = std::move(f_opt).assume_value();

  f.seekp(seek);

  return WALWriterImpl(std::move(executor), std::move(path), std::move(f));
}

fs::path WALWriterImpl::GetPath() const { return path_; }

}  // namespace stewkk::db::logic::recovery
