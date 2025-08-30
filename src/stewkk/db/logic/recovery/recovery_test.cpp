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

  auto [persistent_collection, memstorage] = InitializeStorages(1).value();

  auto got1 = persistent_collection.Get("a").value().value;
  auto got2 = memstorage.Get("c").value().value;

  ASSERT_THAT(got1, Eq("b"));
  ASSERT_THAT(got2, Eq("d"));
}

}  // namespace stewkk::db::logic::recovery
