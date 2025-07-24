#pragma once

#include <string>
#include <unordered_map>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using models::storage::KwPair;
using result::Result;

class MemoryStorage {
  using Map = std::unordered_map<std::string, std::string>;
  using ConstIterator = Map::const_iterator;

public:
  Result<KwPair> Get(std::string key);
  Result<> Remove(std::string key);
  void Insert(KwPair data);
  Result<> Update(KwPair data);

  ConstIterator begin();
  ConstIterator end();

private:
  Map map_;
};

}  // namespace stewkk::db::logic::storage
