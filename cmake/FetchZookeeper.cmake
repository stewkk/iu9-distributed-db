include(FetchContent)
include(ExternalProject)

ExternalProject_Add(
  zookeeper
  GIT_REPOSITORY "https://github.com/apache/zookeeper.git"
  GIT_TAG release-3.8.4
  GIT_PROGRESS   TRUE
  GIT_SHALLOW    TRUE
  USES_TERMINAL_DOWNLOAD TRUE
  CONFIGURE_COMMAND mvn generate-sources
  BUILD_COMMAND cmake zookeeper-client/zookeeper-client-c && make zookeeper
  INSTALL_COMMAND :
  BUILD_IN_SOURCE 1
)

ExternalProject_Get_Property(zookeeper SOURCE_DIR)
add_library(zookeeper-client INTERFACE)
target_include_directories(zookeeper-client INTERFACE ${SOURCE_DIR}/zookeeper-client/zookeeper-client-c/include ${SOURCE_DIR}/zookeeper-client/zookeeper-client-c/generated)
target_link_directories(zookeeper-client INTERFACE ${SOURCE_DIR})
target_link_libraries(zookeeper-client INTERFACE zookeeper hashtable)
