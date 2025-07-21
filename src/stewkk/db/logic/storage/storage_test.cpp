#include <gmock/gmock.h>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/storage.hpp>

using ::testing::Eq;
using ::testing::IsTrue;

namespace stewkk::db::logic::storage {

TEST(StorageTest, Get) {
  MapStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);

  auto got = storage.Get("key");

  ASSERT_THAT(got.value(), Eq(data));
}

TEST(StorageTest, GetNonExisting) {
  MapStorage storage;

  auto got = storage.Get("abc").error();

  ASSERT_THAT(result::What(got), Eq("key abc not found in storage"));
}

TEST(StorageTest, GetAfterRemove) {
  MapStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);
  auto _ = storage.Remove("key");

  auto got = storage.Get("key");

  ASSERT_THAT(got.has_error(), IsTrue);
}

TEST(StorageTest, RemoveNonExisting) {
  MapStorage storage;

  auto got = storage.Remove("abc").error();

  ASSERT_THAT(result::What(got), Eq("key abc not found in storage"));
}

TEST(StorageTest, InsertUpdatesExistingKey) {
  MapStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);
  data.value = "other";

  storage.Insert(data);
  auto got = storage.Get("key");

  ASSERT_THAT(got.value().value, Eq("other"));
}

}  // namespace stewkk::db::logic::storage
