#pragma once

#include <boost/asio/executor.hpp>

#include <stewkk/db/logic/recovery/swappable_wal_writer.hpp>
#include <stewkk/db/logic/recovery/wal_writer_impl.hpp>
#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::recovery {

class SwappableWalWriterImpl : public WALWriter, public SwappableWalWriter {
public:
  SwappableWalWriterImpl(WALWriterImpl* writer, boost::asio::any_io_executor);

  virtual Result<> Remove(const boost::asio::yield_context& yield, std::string key,
                          uint64_t version) override;
  virtual Result<> Insert(const boost::asio::yield_context& yield, KwPair data) override;

  virtual Result<RemoveWalCallback> Swap(const boost::asio::yield_context& yield) override;

  fs::path GetPath(const boost::asio::yield_context& yield) const;

private:
  Result<> WriteEntry(const boost::asio::yield_context& yield, wal::Entry&& entry);

private:
  std::unique_ptr<WALWriterImpl> writer_;
  boost::asio::any_io_executor executor_;
  boost::asio::strand<boost::asio::any_io_executor> strand_;
};

result::Result<SwappableWalWriterImpl> NewSwappableWalWriter(boost::asio::any_io_executor executor);

result::Result<SwappableWalWriterImpl> LoadSwappableWalWriter(boost::asio::any_io_executor executor,
                                                              fs::path path, int64_t seek);

}  // namespace stewkk::db::logic::recovery
