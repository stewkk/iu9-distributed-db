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
)
target_link_libraries(folly_target PUBLIC
    glog_target
    double_conversion_target
    Boost::preprocessor
)
