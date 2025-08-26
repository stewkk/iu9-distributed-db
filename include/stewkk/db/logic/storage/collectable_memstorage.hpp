#pragma once

#include <vector>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using models::storage::KwPair;
using result::Result;

class CollectableStorage {
public:
  virtual std::vector<KwPair> Collect() = 0;

  virtual ~CollectableStorage() = default;
};

}  // namespace stewkk::db::logic::storage
