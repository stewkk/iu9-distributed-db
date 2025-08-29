#include <gmock/gmock.h>

#include <boost/asio/spawn.hpp>
#include <boost/asio/thread_pool.hpp>

#include <stewkk/db/logic/filesystem/filesystem.hpp>
#include <stewkk/db/logic/recovery/swappable_wal_writer_impl.hpp>
#include <stewkk/db/logic/recovery/wal_reader.hpp>
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
    writer.Remove(yield, "blabla").value();
    writer.Insert(yield, KwPair{"a", "b"}).value();
  });
  pool.join();
  writer.Swap().value();
}

}  // namespace stewkk::db::logic::recovery
