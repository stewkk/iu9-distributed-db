include(FetchGTest)
include(GoogleTest)

add_executable(unittests)
target_sources(unittests PRIVATE
  ${PROJECT_SOURCE_DIR}/src/stewkk/db/logic/storage/inmemory_test.cpp
  ${PROJECT_SOURCE_DIR}/src/stewkk/db/logic/storage/swappable_test.cpp
  ${PROJECT_SOURCE_DIR}/src/stewkk/db/logic/storage/persistent_test.cpp
  ${PROJECT_SOURCE_DIR}/src/stewkk/db/logic/recovery/wal_test.cpp
  ${PROJECT_SOURCE_DIR}/src/stewkk/db/logic/recovery/recovery_test.cpp
)
target_compile_options(unittests PRIVATE ${BASE_COMPILE_FLAGS})
target_link_options(unittests PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(unittests PRIVATE
  gmock_main
  stewkk::libdb
  wal_grpc_proto
  persistent_proto
)
target_include_directories(unittests PRIVATE "${CMAKE_BINARY_DIR}/src/stewkk/db")
gtest_discover_tests(unittests)
