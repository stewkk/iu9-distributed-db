#pragma once

#include <boost/asio/executor.hpp>

#include <stewkk/db/logic/recovery/swappable_wal_writer.hpp>
#include <stewkk/db/logic/recovery/wal_writer_impl.hpp>
#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::recovery {

class SwappableWalWriterImpl : public WALWriter, public SwappableWalWriter {
public:
  SwappableWalWriterImpl(WALWriterImpl* writer, boost::asio::any_io_executor);
  SwappableWalWriterImpl(const SwappableWalWriterImpl& other) = delete;
  SwappableWalWriterImpl(SwappableWalWriterImpl&& other);
  SwappableWalWriterImpl& operator=(const SwappableWalWriterImpl& other) = delete;
  SwappableWalWriterImpl& operator=(SwappableWalWriterImpl&& other);

  virtual Result<> Remove(const boost::asio::yield_context& yield, std::string key) override;
  virtual Result<> Insert(const boost::asio::yield_context& yield, KwPair data) override;

  virtual Result<RemoveWalCallback> Swap() override;

  fs::path GetPath() const;

  virtual ~SwappableWalWriterImpl();

private:
  std::atomic<WALWriterImpl*> writer_;
  boost::asio::any_io_executor executor_;
};

result::Result<SwappableWalWriterImpl> NewSwappableWalWriter(boost::asio::any_io_executor executor);

result::Result<SwappableWalWriterImpl> LoadSwappableWalWriter(boost::asio::any_io_executor executor,
                                                              fs::path path, int64_t seek);

}  // namespace stewkk::db::logic::recovery
