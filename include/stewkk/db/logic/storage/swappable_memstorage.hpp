#pragma once

#include <atomic>
#include <string>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/collectable_memstorage.hpp>
#include <stewkk/db/logic/storage/memstorage.hpp>
#include <stewkk/db/logic/storage/memstorage_impl.hpp>

namespace stewkk::db::logic::storage {

class SwappableMemoryStorage : public KwStorage, public CollectableStorage {
public:
  virtual Result<KwPair> Get(std::string key) override;
  virtual Result<> Remove(std::string key) override;
  virtual void Insert(KwPair data) override;
  virtual Result<> Update(KwPair data) override;

  virtual std::vector<KwPair> Collect() override;

private:
  std::atomic<MapStorage*> storage_ptr_;
};

}  // namespace stewkk::db::logic::storage
