#include <gmock/gmock.h>

#include <format>

#include <boost/asio/thread_pool.hpp>

#include <stewkk/db/logic/storage/memstorage_impl.hpp>
#include <stewkk/db/logic/storage/persistent.hpp>
#include <stewkk/db/logic/storage/persistent_collection.hpp>

using ::testing::Eq;

namespace stewkk::db::logic::storage {

std::vector<StorageEntry> InitStorage() {
  MapStorage memory;
  for (size_t i = 0; i < 10; i++) {
    memory.Insert({
        std::format("key{}", std::to_string(i)),
        std::format("value{}", std::to_string(i)),
    });
  }
  auto map = memory.GetUnderlying();
  return ToEntries(std::move(map));
}

TEST(PersistentStorageTest, Get) {
  PersistentStorage persistent = NewPersistentStorage(InitStorage()).value();

  auto got = persistent.Get("key4");

  ASSERT_THAT(got.value().value, Eq("value4"));
}

TEST(PersistentStorageTest, Erased) {
  MapStorage memory;
  memory.Insert({"key", "value"});
  memory.Remove("key");
  auto map = memory.GetUnderlying();
  PersistentStorage persistent = NewPersistentStorage(ToEntries(std::move(map))).value();

  auto got = persistent.Get("key");

  ASSERT_THAT(got.value().value, Eq(std::nullopt));
}

TEST(PersistentStorageTest, SaveAndLoad) {
  fs::path path;
  {
    auto persistent = NewPersistentStorage(InitStorage()).value();
    path = persistent.Path();
  }
  auto persistent = LoadPersistentStorage(path).value();

  auto got = persistent.Get("key4");

  ASSERT_THAT(got.value().value, Eq("value4"));
}

TEST(CollectionTest, Read) {
  fs::remove_all(filesystem::GetDataDir());
  fs::create_directory(filesystem::GetDataDir());
  boost::asio::thread_pool pool(1);
  PersistentStorageCollection collection(pool.get_executor());
  auto storage = NewPersistentStorage(InitStorage()).value();
  collection.Add(std::move(storage)).value();

  Result<StorageEntry> got{result::MakeError("failed")};
  boost::asio::spawn(
      pool, [&](const boost::asio::yield_context yield) { got = collection.Get("key4", yield); });
  pool.join();

  ASSERT_THAT(got.value().value, Eq("value4"));
}

TEST(CollectionTest, ReadExisting) {
  fs::remove_all(filesystem::GetDataDir());
  fs::create_directory(filesystem::GetDataDir());
  boost::asio::thread_pool pool(1);
  {
    PersistentStorageCollection collection(pool.get_executor());
    auto storage = NewPersistentStorage(InitStorage()).value();
    collection.Add(std::move(storage)).value();
  }
  PersistentStorageCollection collection(pool.get_executor());

  Result<StorageEntry> got{result::MakeError("failed")};
  boost::asio::spawn(
      pool, [&](const boost::asio::yield_context yield) { got = collection.Get("key4", yield); });
  pool.join();

  ASSERT_THAT(got.value().value, Eq("value4"));
}

}  // namespace stewkk::db::logic::storage
