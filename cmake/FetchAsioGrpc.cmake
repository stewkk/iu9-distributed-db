
include(FetchContent)
include(FetchGRPC)

FetchContent_Declare(
  asio-grpc
  GIT_REPOSITORY https://github.com/Tradias/asio-grpc.git
  GIT_TAG        v3.5.0
  GIT_PROGRESS   TRUE
  GIT_SHALLOW    TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
set(FETCHCONTENT_QUIET OFF)

FetchContent_MakeAvailable(asio-grpc)
