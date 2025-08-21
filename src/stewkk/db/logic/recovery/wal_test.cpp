#include <gmock/gmock.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/recovery/wal_reader.hpp>
#include <stewkk/db/logic/recovery/wal_writer.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

using ::testing::Eq;

namespace stewkk::db::logic::recovery {

TEST(WALTest, WriteAndReadLogs) {
  boost::asio::io_context context;
  fs::path path;
  {
    auto writer = NewWALWriter(context).value();
    boost::asio::spawn(context, [&](boost::asio::yield_context yield) {
      writer.Remove(yield, "blabla").value();
      writer.Insert(yield, KwPair{"a", "b"}).value();
      writer.Update(yield, KwPair{"c", "d"}).value();
    });
    context.run();
    path = writer.GetPath();
  }

  auto got = ReadWAL(path).value();

  ASSERT_THAT(got, Eq(std::vector{
                       Operation{OperationType::kRemove, "blabla", std::nullopt},
                       Operation{OperationType::kInsert, "a", "b"},
                       Operation{OperationType::kUpdate, "c", "d"},
                   }));
}

TEST(WALTest, Concurrency) {}

}  // namespace stewkk::db::logic::recovery
