#pragma once

#include <stewkk/db/logic/controllers/clear.hpp>
#include <stewkk/db/logic/controllers/get.hpp>
#include <stewkk/db/logic/controllers/insert.hpp>
#include <stewkk/db/logic/controllers/remove.hpp>
#include <stewkk/db/logic/controllers/update.hpp>
#include <stewkk/db/logic/recovery/wal_writer.hpp>
#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/memstorage.hpp>

namespace stewkk::db::logic::controllers {

class Controller : public GetController,
                   public InsertController,
                   public RemoveController,
                   public UpdateController,
                   public ClearController {
public:
  Controller(storage::KwStorage& storage, recovery::WALWriter& wal_writer);

  virtual result::Result<> Insert(const boost::asio::yield_context& yield, KwDTO data) override;
  virtual result::Result<> Update(const boost::asio::yield_context& yield, KwDTO data) override;
  virtual result::Result<> Remove(const boost::asio::yield_context& yield, KeyDTO data) override;
  virtual result::Result<ValueDTO> Get(KeyDTO data) override;
  virtual result::Result<> Clear() override;

private:
  storage::KwStorage& storage_;
  recovery::WALWriter& wal_writer_;
};

}  // namespace stewkk::db::logic::controllers
