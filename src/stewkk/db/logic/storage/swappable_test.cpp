#include <gmock/gmock.h>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/swappable_memstorage.hpp>

using ::testing::Eq;

namespace stewkk::db::logic::storage {

TEST(SwappableStorageTest, Get) {
  SwappableMemoryStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);

  auto got = storage.Get("key");

  ASSERT_THAT(got.value(), Eq(data));
}

TEST(SwappableStorageTest, Collect) {
  SwappableMemoryStorage storage;
  storage.Insert(KwPair{.key = "key0", .value = "value0"});
  storage.Insert(KwPair{.key = "key1", .value = "value1"});
  storage.Insert(KwPair{.key = "key2", .value = "value2"});
  storage.Insert(KwPair{.key = "key3", .value = "value3"});
  storage.Insert(KwPair{.key = "key4", .value = "value4"});

  auto got = storage.Collect();

  ASSERT_THAT(got, Eq(std::vector<KwPair>{
                       KwPair{.key = "key0", .value = "value0"},
                       KwPair{.key = "key1", .value = "value1"},
                       KwPair{.key = "key2", .value = "value2"},
                       KwPair{.key = "key3", .value = "value3"},
                       KwPair{.key = "key4", .value = "value4"},
                   }));
}

TEST(SwappableStorageTest, GetAfterCollect) {
  SwappableMemoryStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);
  auto _ = storage.Collect();

  auto got = storage.Get("key");

  ASSERT_THAT(got.has_failure(), true);
}

TEST(SwappableStorageTest, InsertAfterCollect) {
  SwappableMemoryStorage storage;
  storage.Insert(KwPair{.key = "key0", .value = "value0"});
  auto _ = storage.Collect();
  storage.Insert(KwPair{.key = "key1", .value = "value1"});

  auto got = storage.Get("key1").value();

  ASSERT_THAT(got, Eq(KwPair{.key = "key1", .value = "value1"}));
}

}  // namespace stewkk::db::logic::storage
