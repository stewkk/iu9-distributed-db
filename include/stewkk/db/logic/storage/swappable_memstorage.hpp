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
  SwappableMemoryStorage();
  explicit SwappableMemoryStorage(MapStorage* other);
  ~SwappableMemoryStorage();

  SwappableMemoryStorage(const SwappableMemoryStorage& other) = delete;
  SwappableMemoryStorage(SwappableMemoryStorage&& other);
  SwappableMemoryStorage& operator=(const SwappableMemoryStorage& other) = delete;
  SwappableMemoryStorage& operator=(SwappableMemoryStorage&& other);

  virtual Result<std::optional<std::string>> Get(std::string key) override;
  virtual void Remove(std::string key) override;
  virtual void Insert(KwPair data) override;
  virtual void Clear() override;
  virtual size_t Size() const override;

  virtual std::vector<StorageEntry> Collect() override;

private:
  std::atomic<MapStorage*> storage_ptr_;
};

}  // namespace stewkk::db::logic::storage
