#include <gmock/gmock.h>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/memstorage_impl.hpp>

using ::testing::Eq;
using ::testing::Optional;
using ::testing::StrEq;

namespace stewkk::db::logic::storage {

TEST(MemoryStorageTest, Get) {
  MapStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);

  auto got = storage.Get("key");

  ASSERT_THAT(got.value(), Optional(std::string{"value"}));
}

TEST(MemoryStorageTest, GetNonExisting) {
  MapStorage storage;

  auto got = storage.Get("abc").error();

  ASSERT_THAT(result::What(got), StrEq("key abc not found in storage"));
}

TEST(MemoryStorageTest, GetAfterRemove) {
  MapStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);
  storage.Remove("key");

  auto got = storage.Get("key").value();

  ASSERT_THAT(got, Eq(std::nullopt));
}

TEST(MemoryStorageTest, InsertUpdatesExistingKey) {
  MapStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);
  data.value = "other";

  storage.Insert(data);
  auto got = storage.Get("key");

  ASSERT_THAT(got.value().value(), Eq("other"));
}

}  // namespace stewkk::db::logic::storage
