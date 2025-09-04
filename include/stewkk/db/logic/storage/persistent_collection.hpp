#pragma once

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/strand.hpp>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/persistent.hpp>

namespace stewkk::db::logic::storage {

class PersistentStorageCollection {
public:
  explicit PersistentStorageCollection(boost::asio::any_io_executor executor);

  Result<> Add(PersistentStorage&& storage);
  Result<StorageEntry> Get(std::string_view key, const boost::asio::yield_context& yield);
  size_t Size() const;
  Result<> SwapWith(PersistentStorage&& storage, const boost::asio::yield_context& yield);
  const std::vector<PersistentStorage>& GetCollection() const;

private:
  std::vector<PersistentStorage> collection_;
  boost::asio::strand<boost::asio::any_io_executor> strand_;
  boost::asio::any_io_executor executor_;
};

}  // namespace stewkk::db::logic::storage
