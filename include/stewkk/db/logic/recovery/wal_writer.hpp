#pragma once

#include <filesystem>
#include <fstream>

#include <boost/asio/executor.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/thread_pool.hpp>

#include <wal.pb.h>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::recovery {

using models::storage::KwPair;
using result::Result;
namespace fs = std::filesystem;

class WALWriter {
public:
  WALWriter(boost::asio::executor context, fs::path path, std::ofstream&& stream);

  Result<> Remove(boost::asio::yield_context yield, std::string key);
  Result<> Insert(boost::asio::yield_context yield, KwPair data);
  Result<> Update(boost::asio::yield_context yield, KwPair data);

  fs::path GetPath() const;

private:
  Result<> WriteEntry(boost::asio::yield_context yield, wal::Entry entry);

private:
  fs::path path_;
  std::ofstream f_;
  boost::asio::strand<boost::asio::executor> strand_;
  boost::asio::executor executor_;
};

Result<WALWriter> NewWALWriter(boost::asio::executor executor);

}  // namespace stewkk::db::logic::recovery
