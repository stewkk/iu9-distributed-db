#include <stewkk/db/logic/storage/swappable_memstorage.hpp>

namespace stewkk::db::logic::storage {

Result<KwPair> SwappableMemoryStorage::Get(std::string key) {
  return storage_ptr_.load()->Get(std::move(key));
}

Result<> SwappableMemoryStorage::Remove(std::string key) {
  return storage_ptr_.load()->Remove(std::move(key));
}

void SwappableMemoryStorage::Insert(KwPair data) {
  return storage_ptr_.load()->Insert(std::move(data));
}

Result<> SwappableMemoryStorage::Update(KwPair data) {
  return storage_ptr_.load()->Update(std::move(data));
}

std::vector<KwPair> SwappableMemoryStorage::Collect() {
  auto new_storage = std::make_shared<MapStorage>();
  auto current_storage = storage_ptr_.exchange(new_storage);
  // FIXME: data race here!
  return current_storage->Collect();
}

}  // namespace stewkk::db::logic::storage
