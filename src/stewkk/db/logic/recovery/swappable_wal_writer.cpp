#include <stewkk/db/logic/recovery/swappable_wal_writer_impl.hpp>

#include <folly/synchronization/Hazptr.h>

namespace stewkk::db::logic::recovery {

SwappableWalWriterImpl::SwappableWalWriterImpl(WALWriterImpl* writer,
                                               boost::asio::executor executor)
    : writer_(writer), executor_(std::move(executor)) {}

SwappableWalWriterImpl::SwappableWalWriterImpl(SwappableWalWriterImpl&& other)
    : writer_(other.writer_.exchange(nullptr)), executor_(std::move(other.executor_)) {}

SwappableWalWriterImpl& SwappableWalWriterImpl::operator=(SwappableWalWriterImpl&& other) {
  auto current = writer_.exchange(nullptr);
  if (current != nullptr) {
    current->retire();
  }
  writer_ = other.writer_.exchange(nullptr);
  executor_ = std::move(other.executor_);
  return *this;
}

SwappableWalWriterImpl::~SwappableWalWriterImpl() {
  auto ptr = writer_.exchange(nullptr);
  if (ptr != nullptr) {
    ptr->retire();
  }
}

result::Result<SwappableWalWriterImpl> NewSwappableWalWriter(boost::asio::executor executor) {
  auto got = NewWALWriter(executor);
  if (got.has_failure()) {
    return result::WrapError(std::move(got), "failed to create wal file");
  }
  auto& writer = got.assume_value();
  WALWriterImpl* writer_ptr = new WALWriterImpl(std::move(writer));
  return SwappableWalWriterImpl(writer_ptr, executor);
}

Result<> SwappableWalWriterImpl::Remove(const boost::asio::yield_context& yield, std::string key) {
  folly::hazptr_holder h = folly::make_hazard_pointer();
  WALWriterImpl* ptr = h.protect(writer_);
  return ptr->Remove(yield, std::move(key));
}

Result<> SwappableWalWriterImpl::Insert(const boost::asio::yield_context& yield, KwPair data) {
  folly::hazptr_holder h = folly::make_hazard_pointer();
  WALWriterImpl* ptr = h.protect(writer_);
  return ptr->Insert(yield, std::move(data));
}

Result<RemoveWalCallback> SwappableWalWriterImpl::Swap() {
  auto got = NewWALWriter(executor_);
  if (got.has_failure()) {
    return result::WrapError(std::move(got), "failed to create new WAL file");
  }
  auto& new_writer = got.assume_value();
  auto new_writer_ptr = new WALWriterImpl(std::move(new_writer));

  auto current_writer = writer_.exchange(new_writer_ptr);
  auto path = current_writer->GetPath();
  current_writer->retire();

  return [path = std::move(path)]() {
    std::error_code ec;
    fs::remove(path, ec);
  };
}

}  // namespace stewkk::db::logic::recovery
