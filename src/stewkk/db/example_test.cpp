#include <gmock/gmock.h>

using ::testing::Eq;

namespace stewkk::db {

TEST(ExampleTest, APlusB) { ASSERT_THAT(2 + 2, Eq(4)); }

}  // namespace stewkk::db
