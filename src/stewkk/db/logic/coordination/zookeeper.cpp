#include <stewkk/db/logic/coordination/zookeeper.hpp>

#include <absl/log/log.h>
#include <boost/algorithm/string/join.hpp>

namespace stewkk::db::logic::coordination {

namespace {

using std::chrono_literals::operator""s;

constexpr static int kTimeout = std::chrono::milliseconds{5s}.count();
constexpr static std::string kMasterPath = "/master";

static std::string kDbHost;
static zhandle_t* kZh;
static std::string kHost;

static std::atomic_flag kConnected{};
static std::atomic_flag kExpired{};
static std::atomic_flag kIsMaster{};
static std::atomic<std::shared_ptr<std::string>> kMasterNode = nullptr;
static std::atomic<std::shared_ptr<std::vector<std::string>>> kNodes = nullptr;

void LogCallback(const char* message) {
  LOG(INFO) << "ZK: " << message;
  LOG(INFO) << kConnected.test() << ' ' << kExpired.test() << ' ' << kIsMaster.test();
}

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

void CheckMaster();
void TryBecomeMaster();
void BootstrapServiceDiscovery();
void RegisterNode();
void DiscoverNodes();

void SessionWatcher(zhandle_t* zh, int type, int state, const char* path, void* context) {
  if (type == ZOO_SESSION_EVENT) {
    if (state == ZOO_CONNECTED_STATE) {
      auto is_expired = kExpired.test();
      kConnected.test_and_set();
      kExpired.clear();
      LOG(INFO) << "server connected";
      if (is_expired) {
        TryBecomeMaster();
        RegisterNode();
      }
    } else if (state == ZOO_NOTCONNECTED_STATE || state == ZOO_CONNECTING_STATE
               || state == ZOO_ASSOCIATING_STATE) {
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
  } else {
    LOG(INFO) << "got watch event: " << EventTypeToString(type);
  }
}

void SetMasterNode(const char* value) { kMasterNode.store(std::make_shared<std::string>(value)); }

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
        break;
      }
      assert(value);
      LOG(INFO) << "master is: " << value;
      SetMasterNode(value);
      break;
    case ZNONODE:
      LOG(INFO) << "trying to become master";
      TryBecomeMaster();
      break;
    default:
      LOG(ERROR) << "error when checking for existing master: " << zerror(rc);
  }
}

void CheckMasterWatch(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx) {
  if (type == ZOO_DELETED_EVENT) {
    assert(std::string{path} == "/master");
    LOG(INFO) << "current master is gone, trying to become master";
    TryBecomeMaster();
  } else {
    LOG(INFO) << "got event: " << EventTypeToString(type);
  }
}

void CheckMaster() {
  auto rc = zoo_awget(kZh, kMasterPath.c_str(), CheckMasterWatch, nullptr, CheckMasterCompletion,
                      nullptr);
  if (rc != ZOK) {
    LOG(ERROR) << "failed to check for existing master: " << zerror(rc);
  }
}

void TryBecomeMasterCompletion(int rc, const char* value, const void* data) {
  switch (rc) {
    case ZCONNECTIONLOSS:
    case ZOPERATIONTIMEOUT:
      CheckMaster();
      break;
    case ZOK:
      kIsMaster.test_and_set();
      LOG(INFO) << "TryBecomeMaster sucseeded";
      SetMasterNode(kDbHost.c_str());
      break;
    case ZNODEEXISTS:
      LOG(INFO) << "TryBecomeMaster: node already exists";
      CheckMaster();
      break;
    default:
      LOG(ERROR) << "unknown error occured while trying to become master: " << zerror(rc);
      break;
  }
}

void TryBecomeMaster() {
  auto rc = zoo_acreate(kZh, "/master", kDbHost.c_str(), kDbHost.size(), &ZOO_OPEN_ACL_UNSAFE,
                        ZOO_EPHEMERAL, TryBecomeMasterCompletion, nullptr);
  if (rc != ZOK) {
    LOG(ERROR) << "failed do become master: failed to create master node due to unknown error: "
               << zerror(rc);
  }
}

void RegisterNodeCompletion(int rc, const char* value, const void* data) {
  switch (rc) {
    case ZCONNECTIONLOSS:
    case ZOPERATIONTIMEOUT:
      RegisterNode();
      break;
    case ZOK:
      LOG(INFO) << "node registered";
      DiscoverNodes();
      break;
    case ZNODEEXISTS:
      LOG(ERROR) << "node with hostname " << kDbHost << " already exists";
      DiscoverNodes();
      break;
    default:
      LOG(ERROR) << "unknown error occured while registering node: " << zerror(rc);
      break;
  }
}

void RegisterNode() {
  LOG(INFO) << "registering node";
  auto path = std::format("/nodes/{}", kDbHost);
  auto rc = zoo_acreate(kZh, path.c_str(), nullptr, 0, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL,
                        RegisterNodeCompletion, nullptr);
  if (rc != ZOK) {
    LOG(ERROR) << "error while registering node: " << zerror(rc);
  }
}

void DiscoverNodesWatch(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx) {
  if (type == ZOO_CHILD_EVENT) {
    assert(std::string{path} == "/nodes");
    LOG(INFO) << "nodes list changed";
    DiscoverNodes();
  } else {
    LOG(INFO) << "watched event: " << EventTypeToString(type);
  }
}

void SetNodes(std::vector<std::string>&& new_nodes) {
  kNodes.store(std::make_shared<std::vector<std::string>>(std::move(new_nodes)));
}

void DiscoverNodesCompletion(int rc, const struct String_vector* strings, const void* data) {
  switch (rc) {
    case ZCONNECTIONLOSS:
    case ZOPERATIONTIMEOUT:
      DiscoverNodes();
      break;
    case ZOK: {
      assert(strings);
      std::vector<std::string> nodes;
      nodes.reserve(strings->count);
      for (int i = 0; i < strings->count; i++) {
        auto string = strings->data[i];
        assert(string);
        nodes.emplace_back(string);
      }
      LOG(INFO) << "discovered nodes";
      for (auto& node : nodes) {
        LOG(INFO) << "node: " << node;
      }
      SetNodes(std::move(nodes));
      break;
    }
    default:
      LOG(ERROR) << "unknown error occured while discovering nodes: " << zerror(rc);
      break;
  }
}

void DiscoverNodes() {
  LOG(INFO) << "discovering nodes";
  auto rc = zoo_awget_children(kZh, "/nodes", DiscoverNodesWatch, nullptr, DiscoverNodesCompletion,
                               nullptr);
  if (rc != ZOK) {
    LOG(ERROR) << "error while discovering nodes: " << zerror(rc);
  }
}

void BootstrapServiceDiscoveryCompletion(int rc, const char* value, const void* data) {
  switch (rc) {
    case ZCONNECTIONLOSS:
    case ZOPERATIONTIMEOUT:
      BootstrapServiceDiscovery();
      break;
    case ZOK:
      LOG(INFO) << "/nodes zNode created";
      RegisterNode();
      break;
    case ZNODEEXISTS:
      LOG(INFO) << "/nodes already exist";
      RegisterNode();
      break;
    default:
      LOG(ERROR) << "unknown error occured while bootstrapping service discovery: " << zerror(rc);
      break;
  }
}

void BootstrapServiceDiscovery() {
  auto rc = zoo_acreate(kZh, "/nodes", nullptr, 0, &ZOO_OPEN_ACL_UNSAFE, ZOO_PERSISTENT,
                        BootstrapServiceDiscoveryCompletion, nullptr);
  if (rc != ZOK) {
    LOG(ERROR) << "error while bootstrapping service discovery: " << zerror(rc);
  }
}

}  // namespace

result::Result<> InitZookeeper(std::string db_host, std::vector<std::string> zookeeper_hosts) {
  LOG(INFO) << "connecting to zookeeper";
  kDbHost = std::move(db_host);
  kHost = boost::algorithm::join(zookeeper_hosts, ",");
  kZh = zookeeper_init2(kHost.c_str(), SessionWatcher, kTimeout, 0, nullptr, 0, LogCallback);
  if (!kZh) {
    return result::MakeError("failed to connect to zookeeper: failed to init handler");
  }
  BootstrapServiceDiscovery();
  TryBecomeMaster();
  return result::Ok();
}

result::Result<std::string> GetMaster() {
  auto master_ptr = kMasterNode.load();
  if (!master_ptr.get()) {
    return result::MakeError("master not set");
  }
  return *master_ptr;
}

bool IsMaster() { return kIsMaster.test(); }

bool IsConnectionExpired() { return kExpired.test(); }

result::Result<std::vector<std::string>> GetNodes() {
  auto nodes_ptr = kNodes.load();
  if (!nodes_ptr.get()) {
    return result::MakeError("nodes not set");
  }
  return *nodes_ptr;
}

}  // namespace stewkk::db::logic::coordination
