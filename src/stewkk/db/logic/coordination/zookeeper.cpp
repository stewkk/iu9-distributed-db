#include <stewkk/db/logic/coordination/zookeeper.hpp>

#include <absl/log/log.h>
#include <zookeeper/zookeeper.h>

namespace stewkk::db::logic::coordination {

void Init() {
  auto zh = zookeeper_init("localhost:2181", nullptr, 5 * 1000, nullptr, nullptr, 0);
  if (!zh) {
    LOG(ERROR) << "failed to connect to zookeeper";
    return;
  }
  LOG(INFO) << "connected to zookeeper";
}

}  // namespace stewkk::db::logic::coordination
