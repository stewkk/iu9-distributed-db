#include <gmock/gmock.h>

#include <boost/asio/spawn.hpp>
#include <boost/asio/thread_pool.hpp>

#include <stewkk/db/logic/recovery/wal_reader.hpp>
#include <stewkk/db/logic/recovery/wal_writer.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

using ::testing::Eq;

namespace stewkk::db::logic::recovery {

TEST(WALTest, WriteAndReadLogs) {
  boost::asio::thread_pool pool{1};
  fs::path path;
  {
    auto writer = NewWALWriter(pool.get_executor()).value();
    boost::asio::spawn(pool, [&](boost::asio::yield_context yield) {
      writer.Remove(yield, "blabla").value();
      writer.Insert(yield, KwPair{"a", "b"}).value();
      writer.Update(yield, KwPair{"c", "d"}).value();
    });
    pool.join();
    path = writer.GetPath();
  }

  auto got = ReadWAL(path).value();

  ASSERT_THAT(got, Eq(std::vector{
                       Operation{OperationType::kRemove, "blabla", std::nullopt},
                       Operation{OperationType::kInsert, "a", "b"},
                       Operation{OperationType::kUpdate, "c", "d"},
                   }));
}

TEST(WALTest, Concurrent) {
  boost::asio::thread_pool pool;

  fs::path path;
  {
    auto writer = NewWALWriter(pool.get_executor()).value();
    path = writer.GetPath();

    for (int i = 0; i < 20; ++i) {
      boost::asio::spawn(pool, [&writer](boost::asio::yield_context yield) {
        writer.Remove(yield, "blabla").value();
        writer.Insert(yield, KwPair{"a", "b"}).value();
        writer.Update(yield, KwPair{"c", "d"}).value();
      });
    }

    pool.join();
  }

  auto got = ReadWAL(path).value();

  for (auto type : {OperationType::kInsert, OperationType::kRemove, OperationType::kUpdate}) {
    auto count = std::count_if(got.begin(), got.end(),
                               [&type](const auto& el) { return el.type == type; });
    ASSERT_THAT(count, Eq(20));
  }
}

}  // namespace stewkk::db::logic::recovery
