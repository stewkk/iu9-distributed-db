include(FetchGRPC)
include(FetchBoost)
include(FetchAsioGrpc)
include(FetchFolly)

get_filename_component(api_proto "api.proto" ABSOLUTE)
get_filename_component(api_proto_path "${api_proto}" PATH)

set(api_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/api.pb.cc")
set(api_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/api.pb.h")
set(api_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/api.grpc.pb.cc")
set(api_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/api.grpc.pb.h")
add_custom_command(
      OUTPUT "${api_proto_srcs}" "${api_proto_hdrs}" "${api_grpc_srcs}" "${api_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${api_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${api_proto}"
      DEPENDS "${api_proto}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_library(api_grpc_proto
  ${api_grpc_srcs}
  ${api_grpc_hdrs}
  ${api_proto_srcs}
  ${api_proto_hdrs})
target_link_libraries(api_grpc_proto
  absl::check
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

get_filename_component(wal_proto "wal.proto" ABSOLUTE)
get_filename_component(wal_proto_path "${wal_proto}" PATH)
set(wal_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/wal.pb.cc")
set(wal_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/wal.pb.h")
set(wal_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/wal.grpc.pb.cc")
set(wal_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/wal.grpc.pb.h")
add_custom_command(
      OUTPUT "${wal_proto_srcs}" "${wal_proto_hdrs}" "${wal_grpc_srcs}" "${wal_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${wal_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${wal_proto}"
      DEPENDS "${wal_proto}")
add_library(wal_grpc_proto
  ${wal_grpc_srcs}
  ${wal_grpc_hdrs}
  ${wal_proto_srcs}
  ${wal_proto_hdrs})
target_link_libraries(wal_grpc_proto
  absl::check
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

add_library(libdb
  logic/storage/inmemory.cpp
  logic/storage/persistent.cpp
  logic/storage/swappable_memory_storage.cpp
  logic/filesystem/filesystem.cpp
  logic/recovery/wal_writer.cpp
  logic/recovery/wal_reader.cpp
  logic/result/result.cpp
  logic/controllers/insert.cpp
  logic/controllers/update.cpp
  logic/controllers/remove.cpp
  logic/controllers/get.cpp
  logic/controllers/clear.cpp
  logic/controllers/controller.cpp
  views/register_handlers.cpp
  views/handlers_proxy.cpp
  views/insert.cpp
  views/update.cpp
  views/remove.cpp
  views/get.cpp
  views/clear.cpp
)
add_library(stewkk::libdb ALIAS libdb)
target_compile_features(libdb PUBLIC cxx_std_20)
set_target_properties(libdb PROPERTIES
  CXX_STANDART 20
  CXX_STANDART_REQUIRED YES
  CXX_EXTENSIONS YES
)
set_target_properties(libdb PROPERTIES OUTPUT_NAME stewkk_db)
target_include_directories(
  libdb PUBLIC $<INSTALL_INTERFACE:include>
                    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
target_compile_options(libdb PRIVATE ${BASE_COMPILE_FLAGS})
target_link_options(libdb PRIVATE ${BASE_LINK_FLAGS})
target_link_libraries(libdb
  Boost::outcome
  Boost::uuid
  Boost::unordered
  Boost::asio
  Boost::thread
  absl::log
  absl::log_initialize
  asio-grpc::asio-grpc
  folly_target
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
)

add_executable(iu9-db main.cpp)
target_compile_features(iu9-db PRIVATE cxx_std_20)
set_target_properties(iu9-db PROPERTIES
  CXX_STANDART 20
  CXX_STANDART_REQUIRED YES
  CXX_EXTENSIONS YES
)
target_compile_options(iu9-db PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(iu9-db
  stewkk::libdb
  asio-grpc::asio-grpc
  api_grpc_proto
  wal_grpc_proto
  absl::check
  absl::flags
  absl::flags_parse
  absl::log
  absl::log_initialize
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
