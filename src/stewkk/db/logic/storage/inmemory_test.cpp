#include <gmock/gmock.h>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/inmemory.hpp>

using ::testing::Eq;
using ::testing::IsTrue;
using ::testing::StrEq;

namespace stewkk::db::logic::storage {

TEST(MemoryStorageTest, Get) {
  MemoryStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);

  auto got = storage.Get("key");

  ASSERT_THAT(got.value(), Eq(data));
}

TEST(MemoryStorageTest, GetNonExisting) {
  MemoryStorage storage;

  auto got = storage.Get("abc").error();

  ASSERT_THAT(result::What(got), StrEq("key abc not found in storage"));
}

TEST(MemoryStorageTest, GetAfterRemove) {
  MemoryStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);
  auto _ = storage.Remove("key");

  auto got = storage.Get("key");

  ASSERT_THAT(got.has_error(), IsTrue);
}

TEST(MemoryStorageTest, RemoveNonExisting) {
  MemoryStorage storage;

  auto got = storage.Remove("abc").error();

  ASSERT_THAT(result::What(got), StrEq("key abc not found in storage"));
}

TEST(MemoryStorageTest, InsertUpdatesExistingKey) {
  MemoryStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);
  data.value = "other";

  storage.Insert(data);
  auto got = storage.Get("key");

  ASSERT_THAT(got.value().value, Eq("other"));
}

TEST(MemoryStorageTest, Update) {
  MemoryStorage storage;
  KwPair data{.key = "key", .value = "value"};
  storage.Insert(data);
  data.value = "other";

  auto _ = storage.Update(data);
  auto got = storage.Get("key");

  ASSERT_THAT(got.value().value, Eq("other"));
}

TEST(MemoryStorageTest, UpdateFailsOnNonExistingKey) {
  MemoryStorage storage;
  KwPair data{.key = "abc", .value = "value"};

  auto got = storage.Update(data).error();

  ASSERT_THAT(result::What(got), StrEq("trying to update non-existing key abc"));
}

}  // namespace stewkk::db::logic::storage
