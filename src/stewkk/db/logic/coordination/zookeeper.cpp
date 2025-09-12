#include <stewkk/db/logic/coordination/zookeeper.hpp>

#include <absl/log/log.h>
#include <boost/algorithm/string/join.hpp>

namespace stewkk::db::logic::coordination {

namespace {

using std::chrono_literals::operator""s;

constexpr static int kTimeout = std::chrono::milliseconds{10s}.count();
constexpr static std::string kMasterPath = "/master";

static std::string kServerUUID;
static zhandle_t* kZh;  // TODO: synchronization needed?
static std::string kHost;

static std::atomic_flag kConnected{};
static std::atomic_flag kExpired{};
static std::atomic_flag kIsMaster{};
static std::atomic<std::shared_ptr<std::string>> kMasterNode;

static std::mutex kConnectionLock;

void LogCallback(const char* message) { LOG(INFO) << "ZK: " << message; }

std::string EventTypeToString(int type) {
  static const std::map<int, std::string> mapping = {
      {ZOO_CREATED_EVENT, "ZOO_CREATED_EVENT"}, {ZOO_DELETED_EVENT, "ZOO_DELETED_EVENT"},
      {ZOO_CHANGED_EVENT, "ZOO_CHANGED_EVENT"}, {ZOO_CHILD_EVENT, "ZOO_CHILD_EVENT"},
      {ZOO_SESSION_EVENT, "ZOO_SESSION_EVENT"}, {ZOO_NOTWATCHING_EVENT, "ZOO_NOTWATCHING_EVENT"}};
  if (auto it = mapping.find(type); it != mapping.end()) {
    return it->second;
  }
  return "unknown event type";
}

std::string WatchStateToString(int state) {
  static const std::map<int, std::string> mapping
      = {{ZOO_EXPIRED_SESSION_STATE, "ZOO_EXPIRED_SESSION_STATE"},
         {ZOO_AUTH_FAILED_STATE, "ZOO_AUTH_FAILED_STATE"},
         {ZOO_CONNECTING_STATE, "ZOO_CONNECTING_STATE"},
         {ZOO_ASSOCIATING_STATE, "ZOO_ASSOCIATING_STATE"},
         {ZOO_CONNECTED_STATE, "ZOO_CONNECTED_STATE"}};
  if (auto it = mapping.find(state); it != mapping.end()) {
    return it->second;
  }
  return "unknown watch state";
}

void SessionWatcher(zhandle_t* zh, int type, int state, const char* path, void* context) {
  if (type == ZOO_SESSION_EVENT) {
    kConnectionLock.unlock();
    if (state == ZOO_CONNECTED_STATE) {
      kConnected.test_and_set();
      LOG(INFO) << "server connected";
    } else if (state == ZOO_NOTCONNECTED_STATE) {
      kConnected.clear();
      LOG(INFO) << "server disconnected";
    } else if (state == ZOO_EXPIRED_SESSION_STATE) {
      LOG(WARNING) << "zookeeper session expired, reconnecting...";
      kConnected.clear();
      kIsMaster.clear();
      kExpired.test_and_set();
      zookeeper_close(zh);
      kZh = nullptr;
      while (!kZh) {
        kZh = zookeeper_init2(kHost.c_str(), SessionWatcher, kTimeout, 0, nullptr, 0, LogCallback);
      }
    }
  }
}

void CheckMaster();
void CheckExistingMaster();

void CheckMasterCompletion(int rc, const char* value, int value_len, const struct Stat* stat,
                           const void* data) {
  switch (rc) {
    case ZCONNECTIONLOSS:
    case ZOPERATIONTIMEOUT:
      CheckMaster();
      break;
    case ZOK:
      if (stat == NULL) {
        LOG(INFO) << "trying to become master";
        TryBecomeMaster();
      }
      assert(value);
      /// TODO: мы узнаем кто стал мастером
      LOG(INFO) << "master is: " << value;
      break;
    default:
      LOG(ERROR) << "error when checking for existing master: " << zerror(rc);
  }
}

void CheckMaster() {
  auto rc = zoo_aget(kZh, kMasterPath.c_str(), true, CheckMasterCompletion, nullptr);
  if (rc != ZOK) {
    LOG(ERROR) << "failed to check for existing master: " << zerror(rc);
  }
}

void CheckExistingMasterWatch(zhandle_t* zh, int type, int state, const char* path,
                              void* watcherCtx) {
  if (type == ZOO_DELETED_EVENT) {
    assert(std::string{path} == "/master");
    TryBecomeMaster();
  } else {
    LOG(INFO) << "got event: " << EventTypeToString(type);
  }
}

void CheckExistingMasterCompletion(int rc, const struct Stat* stat, const void* data) {
  switch (rc) {
    case ZCONNECTIONLOSS:
    case ZOPERATIONTIMEOUT:
      CheckExistingMaster();
      break;
    case ZOK:
      if (stat == NULL) {
        LOG(INFO) << "trying to become master";
        TryBecomeMaster();
      }
      break;
    default:
      LOG(ERROR) << "error when checking for existing master: " << zerror(rc);
  }
}

void CheckExistingMaster() {
  int rc = zoo_awexists(kZh, kMasterPath.c_str(), CheckExistingMasterWatch, nullptr,
                        CheckExistingMasterCompletion, nullptr);
  if (rc != ZOK) {
    LOG(ERROR) << "error when checking for existing master " << zerror(rc);
  }
}

void TryBecomeMasterCompletion(int rc, const char* value, const void* data) {
  switch (rc) {
    case ZCONNECTIONLOSS:
      CheckMaster();
      break;
    case ZOK:
      kIsMaster.test_and_set();
      LOG(INFO) << "TryBecomeMaster sucseeded";
      break;
    case ZNODEEXISTS:
      if (value) {
        LOG(INFO) << "TryBecomeMasterCompletion NODEEXISTS: " << value;
      } else {
        LOG(ERROR) << "TryBecomeMasterCompletion NODEEXISTS: got nullptr";
      }
      CheckExistingMaster();
      break;
    default:
      LOG(ERROR) << "unknown error occured when trying to become master";
      break;
  }
}

}  // namespace

result::Result<> InitZookeeper(std::vector<std::string> zookeeper_hosts) {
  LOG(INFO) << "connecting to zookeeper";
  kConnectionLock.lock();
  kHost = boost::algorithm::join(zookeeper_hosts, ",");
  kZh = zookeeper_init2(kHost.c_str(), SessionWatcher, kTimeout, 0, nullptr, 0, LogCallback);
  if (!kZh) {
    return result::MakeError("failed to connect to zookeeper: failed to init handler");
  }
  return result::Ok();
}

void TryBecomeMaster() {
  if (!kConnected.test()) {
    LOG(ERROR) << "failed do become master: not connected to zookeeper";
  }

  auto rc = zoo_acreate(kZh, "/master", kServerUUID.c_str(), kServerUUID.size(),
                        &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, TryBecomeMasterCompletion, nullptr);
  if (rc != ZOK) {
    LOG(ERROR) << "failed do become master: failed to create master node due to unknown error: "
               << zerror(rc);
  }
}

void WaitConnection() { kConnectionLock.lock(); }

}  // namespace stewkk::db::logic::coordination
