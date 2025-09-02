#pragma once

#include <boost/asio/executor.hpp>
#include <boost/thread/thread.hpp>

#include <stewkk/db/logic/controllers/clear.hpp>
#include <stewkk/db/logic/controllers/get.hpp>
#include <stewkk/db/logic/controllers/insert.hpp>
#include <stewkk/db/logic/controllers/remove.hpp>
#include <stewkk/db/logic/recovery/swappable_wal_writer.hpp>
#include <stewkk/db/logic/recovery/wal_writer.hpp>
#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/persistent_collection.hpp>
#include <stewkk/db/logic/storage/swappable_memstorage.hpp>
#include <stewkk/db/logic/synchronization/job_guard.hpp>

namespace stewkk::db::logic::controllers {

class Controller : public GetController,
                   public InsertController,
                   public RemoveController,
                   public ClearController {
public:
  Controller(storage::SwappableMemoryStorage& storage, recovery::WALWriter& wal_writer,
             recovery::SwappableWalWriter& swappable_wal_writer,
             storage::PersistentStorageCollection& persistent_storage,
             boost::asio::any_io_executor executor);

  virtual result::Result<> Insert(const boost::asio::yield_context& yield, KwDTO data) override;
  virtual result::Result<> Remove(const boost::asio::yield_context& yield, KeyDTO data) override;
  virtual result::Result<ValueDTO> Get(KeyDTO data) override;
  virtual result::Result<> Clear() override;

private:
  void SwapToPersistentStorage(const boost::asio::yield_context& yield);
  void TriggerSwapToPersistentStorage();

private:
  storage::SwappableMemoryStorage& storage_;
  recovery::WALWriter& wal_writer_;
  recovery::SwappableWalWriter& swappable_wal_writer_;
  storage::PersistentStorageCollection& persistent_storage_;

  boost::asio::any_io_executor executor_;
  synchronization::JobGuard swap_job_guard_;
};

}  // namespace stewkk::db::logic::controllers
