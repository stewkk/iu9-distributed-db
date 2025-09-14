#include <gmock/gmock.h>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/swappable_memstorage.hpp>

using ::testing::Eq;
using ::testing::Optional;

namespace stewkk::db::logic::storage {

TEST(SwappableStorageTest, Get) {
  SwappableMemoryStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);

  auto got = storage.Get("key");

  ASSERT_THAT(got.value().value, Optional(std::string{"value"}));
}

TEST(SwappableStorageTest, Collect) {
  SwappableMemoryStorage storage;
  storage.Insert(KwPair{.key = "key0", .value = "value0"});
  storage.Insert(KwPair{.key = "key1", .value = "value1"});
  storage.Insert(KwPair{.key = "key2", .value = "value2"});
  storage.Insert(KwPair{.key = "key3", .value = "value3"});
  storage.Insert(KwPair{.key = "key4", .value = "value4"});

  auto got = storage.Collect();

  ASSERT_THAT(got, Eq(std::vector<StorageEntry>{
                       {.key = "key0", .value = "value0"},
                       {.key = "key1", .value = "value1"},
                       {.key = "key2", .value = "value2"},
                       {.key = "key3", .value = "value3"},
                       {.key = "key4", .value = "value4"},
                   }));
}

}  // namespace stewkk::db::logic::storage
