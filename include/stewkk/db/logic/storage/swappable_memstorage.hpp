#pragma once

#include <atomic>
#include <string>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/collectable_memstorage.hpp>
#include <stewkk/db/logic/storage/memstorage.hpp>
#include <stewkk/db/logic/storage/memstorage_impl.hpp>
#include <stewkk/db/logic/synchronization/operations_guard.hpp>

namespace stewkk::db::logic::storage {

class SwappableMemoryStorage : public KwStorage, public CollectableStorage {
public:
  SwappableMemoryStorage();
  explicit SwappableMemoryStorage(MapStorage* other);
  ~SwappableMemoryStorage();

  SwappableMemoryStorage(const SwappableMemoryStorage& other) = delete;
  SwappableMemoryStorage(SwappableMemoryStorage&& other);
  SwappableMemoryStorage& operator=(const SwappableMemoryStorage& other) = delete;
  SwappableMemoryStorage& operator=(SwappableMemoryStorage&& other);

  virtual Result<Value> Get(std::string key) override;
  virtual void Remove(std::string key, uint64_t version) override;
  virtual void Insert(KwPair data) override;
  virtual size_t Size() override;

  virtual std::vector<StorageEntry> Collect() override;
  virtual void RemoveCollected() override;

private:
  std::atomic<synchronization::OwningReferenceCounter<MapStorage>*> reader_first_;
  std::atomic<synchronization::OwningReferenceCounter<MapStorage>*> reader_second_;
  std::atomic<synchronization::ReferenceCounter<MapStorage>*> writer_;
};

}  // namespace stewkk::db::logic::storage
