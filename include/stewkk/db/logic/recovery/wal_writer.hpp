#pragma once

#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::recovery {

using models::storage::KwPair;

class WALWriter {
public:
  KwPair Get(std::string key);
  void Remove(std::string key);
  void Insert(KwPair data);
  void Update(KwPair data);
};

}  // namespace stewkk::db::logic::recovery
