#pragma once

#include <string>
#include <vector>

#include <zookeeper/zookeeper.h>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::coordination {

result::Result<> InitZookeeper(std::string db_host, std::vector<std::string> zookeeper_hosts);

result::Result<std::string> GetMaster();
bool IsMaster();
result::Result<std::vector<std::string>> GetNodes();

}  // namespace stewkk::db::logic::coordination
