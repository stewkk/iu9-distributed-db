#include <gmock/gmock.h>

#include <boost/asio/io_context.hpp>

#include <stewkk/db/logic/recovery/wal_reader.hpp>
#include <stewkk/db/logic/recovery/wal_writer.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

using ::testing::Eq;

namespace stewkk::db::logic::recovery {

TEST(WALTest, WriteAndReadLogs) {
  boost::asio::io_context context;
  auto writer = NewWALWriter(context).value();
  writer.Remove("blabla").value();
  writer.Insert(KwPair{"a", "b"}).value();
  writer.Update(KwPair{"c", "d"}).value();

  auto got = ReadWAL(writer.GetPath());

  ASSERT_THAT(got.value(), Eq(std::vector{
                               Operation{OperationType::kRemove, "blabla", std::nullopt},
                               Operation{OperationType::kInsert, "a", "b"},
                               Operation{OperationType::kUpdate, "c", "d"},
                           }));
}

}  // namespace stewkk::db::logic::recovery
