#pragma once

#include <boost/outcome.hpp>

#include <expected>
#include <string>
#include <unordered_map>

namespace stewkk::db::logic::storage {

struct KwPair {
  std::string key;
  std::string value;
};

class MapStorage {
public:
  KwPair Get(std::string key);
  void Remove(std::string key);
  void Insert(KwPair data);
  void Update(KwPair data);

private:
  std::unordered_map<std::string, std::string> map_;
};

}  // namespace stewkk::db::logic::storage
