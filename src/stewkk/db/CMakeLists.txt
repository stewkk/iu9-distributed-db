include(FetchGRPC)

get_filename_component(my_proto "example.proto" ABSOLUTE)
get_filename_component(my_proto_path "${my_proto}" PATH)

set(my_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/example.pb.cc")
set(my_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/example.pb.h")
set(my_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/example.grpc.pb.cc")
set(my_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/example.grpc.pb.h")
add_custom_command(
      OUTPUT "${my_proto_srcs}" "${my_proto_hdrs}" "${my_grpc_srcs}" "${my_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${my_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${my_proto}"
      DEPENDS "${my_proto}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_library(my_grpc_proto
  ${my_grpc_srcs}
  ${my_grpc_hdrs}
  ${my_proto_srcs}
  ${my_proto_hdrs})
target_link_libraries(my_grpc_proto
  absl::check
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

add_executable(iu9-db main.cpp)
target_compile_features(iu9-db PRIVATE cxx_std_20)
set_target_properties(iu9-db PROPERTIES
  CXX_STANDART 20
  CXX_STANDART_REQUIRED YES
  CXX_EXTENSIONS YES
)
target_compile_options(iu9-db PRIVATE ${BASE_COMPILE_FLAGS})
target_link_libraries(iu9-db
  my_grpc_proto
  absl::check
  absl::flags
  absl::flags_parse
  absl::log
  absl::log_initialize
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
