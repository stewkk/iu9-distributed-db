include(FetchContent)

# FMT
FetchContent_Declare(
    FMT
    GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
    GIT_TAG         11.2.0
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(FMT)
set(fmt_include_dir "${fmt_SOURCE_DIR}/include")
set(fmt_src_dir "${fmt_SOURCE_DIR}/src")
add_library(fmt_target)
target_include_directories(fmt_target PRIVATE
    "${fmt_include_dir}"
)
target_sources(fmt_target PRIVATE
    "${fmt_src_dir}/format.cc"
    "${fmt_src_dir}/os.cc"
)
target_link_libraries(fmt_target PUBLIC fmt::fmt)

# DOUBLE CONVERSION
FetchContent_Declare(
    DOUBLE_CONVERSION
    GIT_REPOSITORY  https://github.com/google/double-conversion.git
    GIT_TAG         v3.1.5
    SOURCE_SUBDIR   double-conversion
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(DOUBLE_CONVERSION)
set(double_conversion_include_dir "${double_conversion_SOURCE_DIR}")
set(double_conversion_src_dir "${double_conversion_SOURCE_DIR}/double-conversion")
add_library(double_conversion_target)
target_include_directories(double_conversion_target PRIVATE
    "${double_conversion_include_dir}"
)
target_sources(double_conversion_target PRIVATE
    "${double_conversion_src_dir}/bignum-dtoa.cc"
    "${double_conversion_src_dir}/bignum.cc"
    "${double_conversion_src_dir}/cached-powers.cc"
    "${double_conversion_src_dir}/diy-fp.cc"
    "${double_conversion_src_dir}/double-conversion.cc"
    "${double_conversion_src_dir}/fast-dtoa.cc"
    "${double_conversion_src_dir}/fixed-dtoa.cc"
    "${double_conversion_src_dir}/strtod.cc"
)

# GLOG
FetchContent_Declare(
    GLOG
    GIT_REPOSITORY  https://github.com/google/glog.git
    GIT_TAG         v0.4.0
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
)
FetchContent_MakeAvailable(GLOG)
set(glog_include_dir "${glog_SOURCE_DIR}/src")
set(glog_src_dir "${glog_SOURCE_DIR}/src")
add_library(glog_target)
target_include_directories(glog_target PUBLIC
    "${glog_BINARY_DIR}"
    "${glog_include_dir}"
    "${glog_include_dir}/base"
)
target_sources(glog_target PRIVATE
    "${glog_src_dir}/demangle.cc"
    "${glog_src_dir}/logging.cc"
    "${glog_src_dir}/raw_logging.cc"
    "${glog_src_dir}/signalhandler.cc"
    "${glog_src_dir}/symbolize.cc"
    "${glog_src_dir}/utilities.cc"
    "${glog_src_dir}/vlog_is_on.cc"
)
target_compile_options(glog_target PRIVATE
    -Wno-shorten-64-to-32
    -Wno-header-hygiene
    -Wno-deprecated-declarations
    -fdeclspec
)

# FOLLY
FetchContent_Declare(
    FOLLY
    GIT_REPOSITORY  https://github.com/facebook/folly.git
    GIT_TAG         v2022.01.24.00
    GIT_PROGRESS   TRUE
    USES_TERMINAL_DOWNLOAD TRUE
    PATCH_COMMAND git apply ${PROJECT_SOURCE_DIR}/cmake/folly-demangle.patch
    UPDATE_DISCONNECTED 1
)
FetchContent_Populate(FOLLY)
set(folly_include_dir "${folly_SOURCE_DIR}")
set(folly_src_dir "${folly_SOURCE_DIR}/folly")
add_library(folly_target)
target_include_directories(folly_target PUBLIC
    "${folly_include_dir}"
    "${boost_include_dirs}"
    "${double_conversion_include_dir}"
    "${glog_include_dir}"
    "${fmt_include_dir}"
)
set(folly_compile_options
    -DFOLLY_NO_CONFIG=1
    -DFOLLY_MOBILE=1
    -DFOLLY_USE_LIBCPP=1
    -DFOLLY_HAVE_RECVMMSG=1
    -DFOLLY_HAVE_PTHREAD=1
    -Wno-comma
    -Wno-shorten-64-to-32
    -Wno-documentation
    -faligned-new
)
target_compile_options(folly_target PUBLIC ${folly_compile_options})
target_sources(folly_target PRIVATE
    "${folly_src_dir}/synchronization/Hazptr.cpp"
    "${folly_src_dir}/synchronization/AsymmetricMemoryBarrier.cpp"
    "${folly_src_dir}/executors/QueuedImmediateExecutor.cpp"
    "${folly_src_dir}/detail/ThreadLocalDetail.cpp"
    "${folly_src_dir}/detail/StaticSingletonManager.cpp"
    "${folly_src_dir}/detail/AtFork.cpp"
    "${folly_src_dir}/system/ThreadId.cpp"
    "${folly_src_dir}/SharedMutex.cpp"
    "${folly_src_dir}/Executor.cpp"
    "${folly_src_dir}/memory/detail/MallocImpl.cpp"
    "${folly_src_dir}/concurrency/CacheLocality.cpp"
    "${folly_src_dir}/detail/Futex.cpp"
    "${folly_src_dir}/synchronization/ParkingLot.cpp"
    "${folly_src_dir}/lang/SafeAssert.cpp"
    "${folly_src_dir}/lang/ToAscii.cpp"
    "${folly_src_dir}/ExceptionWrapper.cpp"
    "${folly_src_dir}/ExceptionString.cpp"
    "${folly_src_dir}/Demangle.cpp"
    "${folly_src_dir}/lang/Exception.cpp"
    "${folly_src_dir}/lang/CString.cpp"
)

find_path(LIBIBERTY_INCLUDE_DIR NAMES libiberty.h PATH_SUFFIXES libiberty)
message(STATUS ${LIBIBERTY_INCLUDE_DIR})

find_library(LIBIBERTY_LIBRARY NAMES iberty libiberty)
message(STATUS ${LIBIBERTY_LIBRARY})

set(LIBIBERTY_FOUND TRUE)

target_include_directories(folly_target PUBLIC ${LIBIBERTY_INCLUDE_DIR})

target_link_libraries(folly_target PUBLIC
    glog_target
    double_conversion_target
    Boost::preprocessor
    ${LIBIBERTY_LIBRARY}
    fmt::fmt
)
