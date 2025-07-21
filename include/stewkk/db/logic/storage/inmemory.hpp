#pragma once

#include <string>
#include <unordered_map>

#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::storage {

using result::Result;

struct KwPair {
  std::string key;
  std::string value;

  bool operator==(const KwPair& other) const = default;
};

class MapStorage {
public:
  Result<KwPair> Get(std::string key);
  Result<> Remove(std::string key);
  void Insert(KwPair data);
  Result<> Update(KwPair data);

private:
  std::unordered_map<std::string, std::string> map_;
};

}  // namespace stewkk::db::logic::storage
