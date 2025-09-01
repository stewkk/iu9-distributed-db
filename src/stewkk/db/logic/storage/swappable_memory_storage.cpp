#include <stewkk/db/logic/storage/swappable_memstorage.hpp>

#include <folly/synchronization/Hazptr.h>

namespace stewkk::db::logic::storage {

SwappableMemoryStorage::SwappableMemoryStorage() : storage_ptr_(new MapStorage) {}

SwappableMemoryStorage::SwappableMemoryStorage(MapStorage* other) : storage_ptr_(other) {}

SwappableMemoryStorage::~SwappableMemoryStorage() {
  auto ptr = storage_ptr_.exchange(nullptr);
  if (ptr != nullptr) {
    ptr->retire();
  }
}

SwappableMemoryStorage::SwappableMemoryStorage(SwappableMemoryStorage&& other)
    : storage_ptr_(other.storage_ptr_.exchange(nullptr)) {}

SwappableMemoryStorage& SwappableMemoryStorage::operator=(SwappableMemoryStorage&& other) {
  auto current = storage_ptr_.exchange(nullptr);
  if (current != nullptr) {
    current->retire();
  }
  storage_ptr_ = other.storage_ptr_.exchange(nullptr);
  return *this;
}

Result<std::optional<std::string>> SwappableMemoryStorage::Get(std::string key) {
  folly::hazptr_holder h = folly::make_hazard_pointer();
  MapStorage* ptr = h.protect(storage_ptr_);
  return ptr->Get(std::move(key));
}

void SwappableMemoryStorage::Remove(std::string key) {
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

size_t SwappableMemoryStorage::Size() const {
  folly::hazptr_holder h = folly::make_hazard_pointer();
  MapStorage* ptr = h.protect(storage_ptr_);
  return ptr->Size();
}

std::vector<StorageEntry> SwappableMemoryStorage::Collect() {
  auto new_storage = new MapStorage;
  auto current_storage = storage_ptr_.exchange(new_storage);
  auto res = current_storage->Collect();
  current_storage->retire();
  return res;
}

}  // namespace stewkk::db::logic::storage
