#include <stewkk/db/logic/recovery/swappable_wal_writer_impl.hpp>

#include <wal.pb.h>

namespace stewkk::db::logic::recovery {

SwappableWalWriterImpl::SwappableWalWriterImpl(WALWriterImpl* writer,
                                               boost::asio::any_io_executor executor)
    : writer_(writer),
      executor_(std::move(executor)),
      strand_(boost::asio::make_strand(executor_)) {}

result::Result<SwappableWalWriterImpl> NewSwappableWalWriter(
    boost::asio::any_io_executor executor) {
  auto got = NewWALWriter();
  if (got.has_failure()) {
    return result::WrapError(std::move(got), "failed to create wal file");
  }
  auto& writer = got.assume_value();
  WALWriterImpl* writer_ptr = new WALWriterImpl(std::move(writer));
  return SwappableWalWriterImpl(writer_ptr, executor);
}

result::Result<SwappableWalWriterImpl> LoadSwappableWalWriter(boost::asio::any_io_executor executor,
                                                              fs::path path, int64_t seek) {
  auto got = LoadWALWriter(path, seek);
  if (got.has_failure()) {
    return result::WrapError(std::move(got), "failed to create wal file");
  }
  auto& writer = got.assume_value();
  WALWriterImpl* writer_ptr = new WALWriterImpl(std::move(writer));
  return SwappableWalWriterImpl(writer_ptr, executor);
}

Result<> SwappableWalWriterImpl::Remove(const boost::asio::yield_context& yield, std::string key,
                                        uint64_t version) {
  wal::Entry entry;
  auto* remove_op = entry.mutable_remove();
  remove_op->set_key(std::move(key));
  remove_op->set_version(std::move(version));

  return WriteEntry(yield, std::move(entry));
}

Result<> SwappableWalWriterImpl::Insert(const boost::asio::yield_context& yield, KwPair data) {
  wal::Entry entry;
  auto* insert_op = entry.mutable_insert();
  insert_op->set_key(std::move(data).key);
  insert_op->set_value(std::move(data).value);
  insert_op->set_version(std::move(data).version);

  return WriteEntry(yield, std::move(entry));
}

Result<> SwappableWalWriterImpl::WriteEntry(const boost::asio::yield_context& yield,
                                            wal::Entry&& entry) {
  boost::asio::post(boost::asio::bind_executor(strand_, yield));
  auto res = writer_->WriteEntry(std::move(entry));
  boost::asio::post(boost::asio::bind_executor(executor_, yield));
  return res;
}

Result<RemoveWalCallback> SwappableWalWriterImpl::Swap(const boost::asio::yield_context& yield) {
  auto got = NewWALWriter();
  if (got.has_failure()) {
    return result::WrapError(std::move(got), "failed to create new WAL file");
  }
  auto new_writer = std::move(got).assume_value();

  auto path = writer_->GetPath();

  boost::asio::post(boost::asio::bind_executor(strand_, yield));
  writer_ = std::make_unique<WALWriterImpl>(std::move(new_writer));
  boost::asio::post(boost::asio::bind_executor(executor_, yield));

  return [path = std::move(path)]() {
    std::error_code ec;
    fs::remove(path, ec);
  };
}

fs::path SwappableWalWriterImpl::GetPath(const boost::asio::yield_context& yield) const {
  boost::asio::post(boost::asio::bind_executor(strand_, yield));
  auto res = writer_->GetPath();
  boost::asio::post(boost::asio::bind_executor(executor_, yield));
  return res;
}

}  // namespace stewkk::db::logic::recovery
