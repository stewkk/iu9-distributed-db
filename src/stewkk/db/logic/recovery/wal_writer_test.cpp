#include <gmock/gmock.h>

#include <stewkk/db/logic/recovery/wal_writer.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

using ::testing::Eq;

namespace stewkk::db::logic::recovery {

TEST(WALWriterTest, WritesLogs) {
  auto writer = NewWALWriter().value();

  writer.Remove("blabla").value();
  writer.Insert(KwPair{}).value();
  writer.Update(KwPair{}).value();
}

}  // namespace stewkk::db::logic::recovery
