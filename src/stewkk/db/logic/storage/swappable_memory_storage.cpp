#include <stewkk/db/logic/storage/swappable_memstorage.hpp>

namespace stewkk::db::logic::storage {

SwappableMemoryStorage::SwappableMemoryStorage()
    : reader_first_(new synchronization::OwningReferenceCounter<MapStorage>()),
      reader_second_(new synchronization::OwningReferenceCounter<MapStorage>()),
      writer_(new synchronization::ReferenceCounter<MapStorage>(
          reader_second_.load()->GetUnderlying())) {}

SwappableMemoryStorage::SwappableMemoryStorage(MapStorage* other)
    : reader_first_(new synchronization::OwningReferenceCounter<MapStorage>()),
      reader_second_(new synchronization::OwningReferenceCounter<MapStorage>(other)),
      writer_(new synchronization::ReferenceCounter<MapStorage>(
          reader_second_.load()->GetUnderlying())) {}

SwappableMemoryStorage::~SwappableMemoryStorage() {
  auto ptr = reader_first_.load();
  if (ptr) {
    delete ptr;
  }
  ptr = reader_second_.load();
  if (ptr) {
    delete ptr;
  }
  auto writer_ptr = writer_.load();
  if (writer_ptr) {
    delete writer_ptr;
  }
}

SwappableMemoryStorage::SwappableMemoryStorage(SwappableMemoryStorage&& other)
    : reader_first_(other.reader_first_.exchange(nullptr)),
      reader_second_(other.reader_second_.exchange(nullptr)),
      writer_(other.writer_.exchange(nullptr)) {}

SwappableMemoryStorage& SwappableMemoryStorage::operator=(SwappableMemoryStorage&& other) {
  reader_first_ = other.reader_first_.exchange(nullptr);
  reader_second_ = other.reader_second_.exchange(nullptr);
  writer_ = other.writer_.exchange(nullptr);
  return *this;
}

Result<Value> SwappableMemoryStorage::Get(std::string key) {
  auto ptr = reader_first_.load();
  auto reader = ptr->Add();
  auto res = reader->Get(key);
  ptr->Remove();
  if (res.has_value()) {
    return res;
  }

  ptr = reader_second_.load();
  reader = ptr->Add();
  res = reader->Get(std::move(key));
  ptr->Remove();

  return res;
}

void SwappableMemoryStorage::Remove(std::string key, uint64_t version) {
  auto ptr = writer_.load();
  auto writer = ptr->Add();
  writer->Remove(std::move(key), std::move(version));
  ptr->Remove();
}

void SwappableMemoryStorage::Insert(KwPair data) {
  auto ptr = writer_.load();
  auto writer = ptr->Add();
  writer->Insert(std::move(data));
  ptr->Remove();
}

size_t SwappableMemoryStorage::Size() {
  auto ptr = writer_.load();
  auto writer = ptr->Add();
  auto res = writer->Size();
  ptr->Remove();
  return res;
}

std::vector<StorageEntry> SwappableMemoryStorage::Collect() {
  // Step 1
  auto new_writer
      = new synchronization::ReferenceCounter<MapStorage>{reader_first_.load()->GetUnderlying()};
  auto old_writer = writer_.exchange(new_writer);

  // Step 2
  delete old_writer;

  // Step 3
  auto data = reader_second_.load()->GetUnderlying()->GetUnderlying();
  return ToEntries(std::move(data));
}

void SwappableMemoryStorage::RemoveCollected() {
  // Step 4
  auto old_reader = reader_second_.exchange(reader_first_.load());
  delete old_reader;

  // Step 5
  reader_first_.store(new synchronization::OwningReferenceCounter<MapStorage>());
}

}  // namespace stewkk::db::logic::storage
