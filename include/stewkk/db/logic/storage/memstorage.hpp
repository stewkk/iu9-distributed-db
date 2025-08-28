#pragma once

#include <string>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using models::storage::KwPair;
using result::Result;

class KwStorage {
public:
  virtual Result<KwPair> Get(std::string key) = 0;
  virtual Result<> Remove(std::string key) = 0;
  virtual void Insert(KwPair data) = 0;
  virtual Result<> Update(KwPair data) = 0;
  virtual void Clear() = 0;

  virtual ~KwStorage() = default;
};

}  // namespace stewkk::db::logic::storage
