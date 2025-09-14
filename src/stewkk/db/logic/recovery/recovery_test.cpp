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
    writer.Remove(yield, "blabla", 0).value();
    writer.Insert(yield, KwPair{"a", "b"}).value();
    writer.Swap(yield).value();
  });
  pool.join();

  auto files = SearchWALFiles().value();

  ASSERT_THAT(files.size(), Eq(2));
}

TEST(RecoveryTest, MemstorageInitialization) {
  fs::remove_all(filesystem::GetDataDir());
  fs::create_directory(filesystem::GetDataDir());
  {
    boost::asio::thread_pool pool{1};
    fs::path path;
    auto writer = NewSwappableWalWriter(pool.get_executor()).value();
    boost::asio::spawn(pool, [&](boost::asio::yield_context yield) {
      writer.Insert(yield, KwPair{"a", "b"}).value();
      writer.Insert(yield, KwPair{"c", "e"}).value();
      writer.Swap(yield).value();
      writer.Insert(yield, KwPair{"c", "d"}).value();
    });
    pool.join();
  }

  boost::asio::thread_pool pool{1};
  auto [persistent_collection, memstorage, wal_writer, _]
      = InitializeStorages(pool.get_executor(), 1).value();

  std::optional<std::string> got1;
  boost::asio::spawn(pool, [&](const boost::asio::yield_context yield) {
    got1 = persistent_collection.Get("a", yield).value().value;
  });
  pool.join();
  auto got2 = memstorage.Get("c").value().value.value();

  ASSERT_THAT(got1.value(), Eq("b"));
  ASSERT_THAT(got2, Eq("d"));
}

TEST(RecoveryTest, WalWriterInitialization) {
  {
    fs::remove_all(filesystem::GetDataDir());
    fs::create_directory(filesystem::GetDataDir());
    boost::asio::thread_pool pool{1};
    fs::path path;
    auto writer = NewSwappableWalWriter(pool.get_executor()).value();
    boost::asio::spawn(pool, [&](boost::asio::yield_context yield) {
      writer.Insert(yield, KwPair{"a", "b"}).value();
      writer.Insert(yield, KwPair{"c", "e"}).value();
      writer.Swap(yield).value();
      writer.Insert(yield, KwPair{"c", "d"}).value();
    });
    pool.join();
  }

  fs::path path;
  {
    boost::asio::thread_pool pool{1};
    auto [_, _, wal_writer, _] = InitializeStorages(pool.get_executor(), 1).value();
    boost::asio::spawn(pool, [&](boost::asio::yield_context yield) {
      wal_writer.Insert(yield, KwPair{"f", "g"}).value();
      path = wal_writer.GetPath(yield);
    });
    pool.join();
  }

  auto got = ReadWAL(path).value().first;

  ASSERT_THAT(got, Eq(std::vector{
                       Operation{OperationType::kInsert, "c", "d"},
                       Operation{OperationType::kInsert, "f", "g"},
                   }));
}

}  // namespace stewkk::db::logic::recovery
