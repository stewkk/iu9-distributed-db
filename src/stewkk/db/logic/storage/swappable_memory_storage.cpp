#include <stewkk/db/logic/storage/swappable_memstorage.hpp>

namespace stewkk::db::logic::storage {

SwappableMemoryStorage::SwappableMemoryStorage()
    : storage_ptr_(new MapStorage), operations_running_(0) {}

SwappableMemoryStorage::SwappableMemoryStorage(MapStorage* other)
    : storage_ptr_(other), operations_running_(0) {}

SwappableMemoryStorage::~SwappableMemoryStorage() {
  auto ptr = storage_ptr_.load();
  if (ptr != nullptr) {
    // TODO: вынести в метод
    while (operations_running_.load()) {
      __builtin_ia32_pause();
    }
    delete ptr;
  }
}

SwappableMemoryStorage::SwappableMemoryStorage(SwappableMemoryStorage&& other)
    : storage_ptr_(other.storage_ptr_.exchange(nullptr)) {}

SwappableMemoryStorage& SwappableMemoryStorage::operator=(SwappableMemoryStorage&& other) {
  auto current = storage_ptr_.load();
  if (current != nullptr) {
    delete current;
  }
  storage_ptr_ = other.storage_ptr_.exchange(nullptr);
  return *this;
}

Result<std::optional<std::string>> SwappableMemoryStorage::Get(std::string key) {
  auto ptr = storage_ptr_.load();
  return ptr->Get(std::move(key));
}

void SwappableMemoryStorage::Remove(std::string key) {
  ++operations_running_;
  storage_ptr_.load()->Remove(std::move(key));
  --operations_running_;
}

void SwappableMemoryStorage::Insert(KwPair data) {
  ++operations_running_;
  storage_ptr_.load()->Insert(std::move(data));
  --operations_running_;
}

void SwappableMemoryStorage::Clear() {
  ++operations_running_;
  storage_ptr_.load()->Clear();
  --operations_running_;
}

size_t SwappableMemoryStorage::Size() {
  ++operations_running_;
  auto res = storage_ptr_.load()->Size();
  --operations_running_;
  return res;
}

std::vector<StorageEntry> SwappableMemoryStorage::Collect() {
  auto new_storage = new MapStorage;
  auto current_storage = storage_ptr_.exchange(new_storage);
  while (operations_running_.load()) {
    __builtin_ia32_pause();
  }
  auto res = current_storage->Collect();
  delete current_storage;
  return res;
}

}  // namespace stewkk::db::logic::storage
