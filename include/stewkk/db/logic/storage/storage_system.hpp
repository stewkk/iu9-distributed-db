#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/recovery/wal_writer.hpp>
#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/persistent_collection.hpp>
#include <stewkk/db/logic/storage/swappable_memstorage.hpp>
#include <stewkk/db/models/dto/dto.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

class StorageSystem {
public:
  StorageSystem(recovery::WALWriter& wal_writer, storage::SwappableMemoryStorage& storage,
                storage::PersistentStorageCollection& persistent_storage);

  result::Result<> Insert(const boost::asio::yield_context& yield, models::storage::KwPair kw_pair);
  result::Result<> Remove(const boost::asio::yield_context& yield, std::string key,
                          uint64_t version);
  result::Result<models::dto::ValueDTO> Get(const boost::asio::yield_context& yield,
                                            std::string key);

private:
  recovery::WALWriter& wal_writer_;
  storage::SwappableMemoryStorage& storage_;
  storage::PersistentStorageCollection& persistent_storage_;
};

}  // namespace stewkk::db::logic::storage
