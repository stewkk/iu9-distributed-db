#pragma once

#include <string>

#include <boost/unordered/concurrent_flat_map.hpp>
#include <boost/unordered/unordered_flat_map.hpp>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/collectable_memstorage.hpp>
#include <stewkk/db/logic/storage/memstorage.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using models::storage::KwPair;
using result::Result;

class MapStorage : public KwStorage {
private:
  using Map = boost::concurrent_flat_map<std::string, std::optional<std::string>>;

public:
  MapStorage() = default;
  explicit MapStorage(Map&& other);

  virtual Result<std::optional<std::string>> Get(std::string key) override;
  virtual void Remove(std::string key) override;
  virtual void Insert(KwPair data) override;
  virtual size_t Size() override;

  const Map& GetUnderlying() const;

private:
  Map map_;
};

std::vector<StorageEntry> ToEntries(
    boost::unordered_flat_map<std::string, std::optional<std::string>> map);

}  // namespace stewkk::db::logic::storage
