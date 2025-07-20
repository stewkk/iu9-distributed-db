#include <gmock/gmock.h>

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

  auto got = storage.Get("key");

  ASSERT_THAT(got.has_error(), IsTrue);
}

}  // namespace stewkk::db::logic::storage
