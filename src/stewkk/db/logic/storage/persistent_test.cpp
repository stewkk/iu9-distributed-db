#include <gmock/gmock.h>

#include <format>

#include <stewkk/db/logic/storage/memstorage_impl.hpp>
#include <stewkk/db/logic/storage/persistent.hpp>

using ::testing::Eq;

namespace stewkk::db::logic::storage {

ReadonlyMemoryStorage InitStorage() {
  MapStorage memory;
  for (size_t i = 0; i < 10; i++) {
    memory.Insert({
        std::format("key{}", std::to_string(i)),
        std::format("value{}", std::to_string(i)),
    });
  }
  return ReadonlyMemoryStorage(std::move(memory));
}

TEST(PersistentStorageTest, Get) {
  PersistentStorage persistent = NewPersistentStorage(InitStorage()).value();

  auto got = persistent.Get("key4");

  ASSERT_THAT(got.value().value, Eq("value4"));
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

}  // namespace stewkk::db::logic::storage
