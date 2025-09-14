#pragma once

#include <optional>
#include <string>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using models::storage::KwPair;
using result::Result;

struct Value {
  std::optional<std::string> value;
  uint64_t version;
};

class KwStorage {
public:
  virtual Result<Value> Get(std::string key) = 0;
  virtual void Remove(std::string key, uint64_t version) = 0;
  virtual void Insert(KwPair data) = 0;
  virtual size_t Size() = 0;

  virtual ~KwStorage() = default;
};

}  // namespace stewkk::db::logic::storage
