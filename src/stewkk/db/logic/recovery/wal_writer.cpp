#include <stewkk/db/logic/recovery/wal_writer.hpp>

#include <absl/log/log.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <wal.pb.h>

#include <stewkk/db/logic/filesystem/filesystem.hpp>

namespace stewkk::db::logic::recovery {

namespace {

static constexpr std::string_view kFailedToCreate = "failed to create WAL";

}  // namespace

WALWriter::WALWriter(boost::asio::io_context& context, fs::path path, std::ofstream&& stream)
    : path_(std::move(path)), f_(std::move(stream)), strand_(boost::asio::make_strand(context)) {}

Result<> WALWriter::Remove(boost::asio::yield_context& yield, std::string key) {
  wal::Entry entry;
  auto* remove_op = entry.mutable_remove();
  remove_op->set_key(std::move(key));
  bool is_ok;
  boost::asio::dispatch(strand_, [&]() {
    is_ok = google::protobuf::util::SerializeDelimitedToOstream(entry, &f_);
    f_.flush();
  });
  if (!is_ok) {
    return result::Error("failed to write log entry");
  }
  return result::Ok();
}

Result<> WALWriter::Insert(boost::asio::yield_context& yield, KwPair data) {
  wal::Entry entry;
  auto* insert_op = entry.mutable_insert();
  insert_op->set_key(std::move(data).key);
  insert_op->set_value(std::move(data).value);
  bool is_ok;
  boost::asio::dispatch(strand_, [&]() {
    is_ok = google::protobuf::util::SerializeDelimitedToOstream(entry, &f_);
    f_.flush();
  });
  if (!is_ok) {
    return result::Error("failed to write log entry");
  }
  return result::Ok();
}

Result<> WALWriter::Update(boost::asio::yield_context& yield, KwPair data) {
  wal::Entry entry;
  auto* update_op = entry.mutable_update();
  update_op->set_key(std::move(data).key);
  update_op->set_value(std::move(data).value);
  bool is_ok;
  boost::asio::dispatch(strand_, [&]() {
    is_ok = google::protobuf::util::SerializeDelimitedToOstream(entry, &f_);
    f_.flush();
  });
  if (!is_ok) {
    return result::Error("failed to write log entry");
  }
  return result::Ok();
}

Result<WALWriter> NewWALWriter(boost::asio::io_context& context) {
  auto extension = "wal";
  auto path = filesystem::GetPath(extension);
  LOG(INFO) << std::format("creating new WAL at {}", path.string());
  auto f_opt = filesystem::CreateBinaryFile(path);
  if (f_opt.has_failure()) {
    return result::WrapError(std::move(f_opt), kFailedToCreate);
  }
  auto f = std::move(f_opt).assume_value();

  return WALWriter(context, std::move(path), std::move(f));
}

fs::path WALWriter::GetPath() const { return path_; }

}  // namespace stewkk::db::logic::recovery
