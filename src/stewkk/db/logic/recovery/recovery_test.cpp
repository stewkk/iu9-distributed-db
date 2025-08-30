#include <gmock/gmock.h>

#include <boost/asio/spawn.hpp>
#include <boost/asio/thread_pool.hpp>

#include <stewkk/db/logic/filesystem/filesystem.hpp>
#include <stewkk/db/logic/recovery/swappable_wal_writer_impl.hpp>
#include <stewkk/db/logic/recovery/wal_reader.hpp>
#include <stewkk/db/logic/storage/persistent_collection.hpp>
#include <stewkk/db/logic/storage/swappable_memstorage.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

using ::testing::Eq;

namespace stewkk::db::logic::recovery {

TEST(RecoveryTest, SearchForWALFiles) {
  fs::remove_all(filesystem::GetDataDir());
  fs::create_directory(filesystem::GetDataDir());
  boost::asio::thread_pool pool{1};
  fs::path path;
  auto writer = NewSwappableWalWriter(pool.get_executor()).value();
  boost::asio::spawn(pool, [&](boost::asio::yield_context yield) {
    writer.Remove(yield, "blabla").value();
    writer.Insert(yield, KwPair{"a", "b"}).value();
  });
  pool.join();
  writer.Swap().value();

  auto files = SearchWALFiles().value();

  ASSERT_THAT(files.size(), Eq(2));
}

void Apply(const std::vector<Operation>& operations, storage::KwStorage& storage) {
  for (auto& operation : operations) {
    switch (operation.type) {
      case OperationType::kInsert:
        storage.Insert(KwPair{operation.key, operation.value.value()});
        break;
      case OperationType::kRemove:
        storage.Remove(operation.key);
        break;
    }
  }
}

TEST(RecoveryTest, MemstorageInitialization) {
  fs::remove_all(filesystem::GetDataDir());
  fs::create_directory(filesystem::GetDataDir());
  boost::asio::thread_pool pool{1};
  fs::path path;
  auto writer = NewSwappableWalWriter(pool.get_executor()).value();
  boost::asio::spawn(pool, [&](boost::asio::yield_context yield) {
    writer.Insert(yield, KwPair{"a", "b"}).value();
    writer.Insert(yield, KwPair{"c", "e"}).value();
    writer.Swap().value();
    writer.Insert(yield, KwPair{"c", "d"}).value();
  });
  pool.join();
  storage::PersistentStorageCollection persistent_collection;
  storage::SwappableMemoryStorage memstorage;

  auto files = SearchWALFiles().value();
  for (const auto& file : files) {
    auto [operations, position] = ReadWAL(file).value();
    storage::MapStorage storage;
    Apply(operations, storage);
    const auto kThreshold = 1;
    if (storage.Size() > kThreshold) {
      auto persistent
          = storage::NewPersistentStorage(storage::ReadonlyMemoryStorage(std::move(storage)))
                .value();
      persistent_collection.Add(std::move(persistent)).value();
    } else {
      storage::MapStorage* ptr = new storage::MapStorage(std::move(storage));
      memstorage = storage::SwappableMemoryStorage{ptr};
    }
  }
  if (files.size() == 2) {
  }
  auto [operations, position] = ReadWAL(files.back()).value();
  Apply(operations, memstorage);

  auto got1 = persistent_collection.Get("a").value().value;
  auto got2 = memstorage.Get("c").value().value;

  ASSERT_THAT(got1, Eq("b"));
  ASSERT_THAT(got2, Eq("d"));
}

}  // namespace stewkk::db::logic::recovery
