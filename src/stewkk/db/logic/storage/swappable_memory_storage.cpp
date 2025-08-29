#include <stewkk/db/logic/storage/swappable_memstorage.hpp>

#include <folly/synchronization/Hazptr.h>

namespace stewkk::db::logic::storage {

SwappableMemoryStorage::SwappableMemoryStorage() : storage_ptr_(new MapStorage) {}

SwappableMemoryStorage::~SwappableMemoryStorage() {
  auto ptr = storage_ptr_.exchange(nullptr);
  ptr->retire();
}

Result<KwPair> SwappableMemoryStorage::Get(std::string key) {
  folly::hazptr_holder h = folly::make_hazard_pointer();
  MapStorage* ptr = h.protect(storage_ptr_);
  return ptr->Get(std::move(key));
}

Result<> SwappableMemoryStorage::Remove(std::string key) {
  folly::hazptr_holder h = folly::make_hazard_pointer();
  MapStorage* ptr = h.protect(storage_ptr_);
  return ptr->Remove(std::move(key));
}

void SwappableMemoryStorage::Insert(KwPair data) {
  folly::hazptr_holder h = folly::make_hazard_pointer();
  MapStorage* ptr = h.protect(storage_ptr_);
  return ptr->Insert(std::move(data));
}

void SwappableMemoryStorage::Clear() {
  folly::hazptr_holder h = folly::make_hazard_pointer();
  MapStorage* ptr = h.protect(storage_ptr_);
  ptr->Clear();
}

std::vector<KwPair> SwappableMemoryStorage::Collect() {
  auto new_storage = new MapStorage;
  auto current_storage = storage_ptr_.exchange(new_storage);
  auto res = current_storage->Collect();
  current_storage->retire();
  return res;
}

}  // namespace stewkk::db::logic::storage
