#pragma once

#include <string>
#include <vector>

#include <zookeeper/zookeeper.h>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::coordination {

result::Result<> InitZookeeper(std::vector<std::string> zookeeper_hosts);

void TryBecomeMaster();

void WaitConnection();

}  // namespace stewkk::db::logic::coordination
