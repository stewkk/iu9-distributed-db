include(FetchContent)

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  URL_MD5 893b5203b862eb9bbd08553e24ff146a
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  EXCLUDE_FROM_ALL
  OVERRIDE_FIND_PACKAGE
)

set(BOOST_INCLUDE_LIBRARIES outcome uuid unordered asio thread scope_exit)

FetchContent_MakeAvailable(Boost)
