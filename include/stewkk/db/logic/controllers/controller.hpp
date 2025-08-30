#pragma once

#include <stewkk/db/logic/controllers/clear.hpp>
#include <stewkk/db/logic/controllers/get.hpp>
#include <stewkk/db/logic/controllers/insert.hpp>
#include <stewkk/db/logic/controllers/remove.hpp>
#include <stewkk/db/logic/recovery/wal_writer.hpp>
#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/memstorage.hpp>
#include <stewkk/db/logic/storage/persistent_collection.hpp>

namespace stewkk::db::logic::controllers {

class Controller : public GetController,
                   public InsertController,
                   public RemoveController,
                   public ClearController {
public:
  Controller(storage::KwStorage& storage, recovery::WALWriter& wal_writer,
             storage::PersistentStorageCollection& persistent_storage);

  virtual result::Result<> Insert(const boost::asio::yield_context& yield, KwDTO data) override;
  virtual result::Result<> Remove(const boost::asio::yield_context& yield, KeyDTO data) override;
  virtual result::Result<ValueDTO> Get(KeyDTO data) override;
  virtual result::Result<> Clear() override;

private:
  storage::KwStorage& storage_;
  recovery::WALWriter& wal_writer_;
  storage::PersistentStorageCollection& persistent_storage_;
};

}  // namespace stewkk::db::logic::controllers
