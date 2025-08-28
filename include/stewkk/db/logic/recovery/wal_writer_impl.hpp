#pragma once

#include <filesystem>
#include <fstream>

#include <boost/asio/executor.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/thread_pool.hpp>

#include <wal.pb.h>

#include <stewkk/db/logic/recovery/wal_writer.hpp>
#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::recovery {

using models::storage::KwPair;
using result::Result;
namespace fs = std::filesystem;

class WALWriterImpl : public WALWriter {
public:
  WALWriterImpl(boost::asio::executor context, fs::path path, std::ofstream&& stream);

  virtual Result<> Remove(const boost::asio::yield_context& yield, std::string key) override;
  virtual Result<> Insert(const boost::asio::yield_context& yield, KwPair data) override;
  virtual Result<> Update(const boost::asio::yield_context& yield, KwPair data) override;

  fs::path GetPath() const;

private:
  Result<> WriteEntry(const boost::asio::yield_context& yield, wal::Entry entry);

private:
  fs::path path_;
  std::ofstream f_;
  boost::asio::strand<boost::asio::executor> strand_;
  boost::asio::executor executor_;
};

Result<WALWriterImpl> NewWALWriter(boost::asio::executor executor);

}  // namespace stewkk::db::logic::recovery
