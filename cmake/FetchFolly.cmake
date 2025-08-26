include(FetchContent)

FetchContent_Declare(
  folly
  GIT_REPOSITORY    https://github.com/facebook/folly.git
  GIT_TAG           c5aa5c46291a27f69acc920894d43605ceb43eba
  GIT_PROGRESS      TRUE
)
FetchContent_Populate(folly)

add_library(folly_headers INTERFACE)
target_include_directories(folly_headers SYSTEM INTERFACE ${folly_SOURCE_DIR})
target_link_libraries(folly_headers INTERFACE Boost::headers)
