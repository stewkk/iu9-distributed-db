include(FetchGTest)
include(GoogleTest)

add_executable(unittests)
target_sources(unittests PRIVATE
  ${PROJECT_SOURCE_DIR}/src/stewkk/db/logic/storage/inmemory_test.cpp
  ${PROJECT_SOURCE_DIR}/src/stewkk/db/logic/storage/persistent_test.cpp
  ${PROJECT_SOURCE_DIR}/src/stewkk/db/logic/recovery/wal_test.cpp
)
target_compile_options(unittests PRIVATE ${BASE_COMPILE_FLAGS})
target_link_options(unittests PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(unittests PRIVATE
  gmock_main
  stewkk::libdb
  wal_grpc_proto
)
gtest_discover_tests(unittests)
