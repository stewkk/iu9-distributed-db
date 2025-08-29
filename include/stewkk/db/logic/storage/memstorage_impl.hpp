#pragma once

#include <string>

#include <folly/synchronization/HazptrObj.h>
#include <boost/unordered/concurrent_flat_map.hpp>
#include <boost/unordered/unordered_flat_map.hpp>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/logic/storage/collectable_memstorage.hpp>
#include <stewkk/db/logic/storage/memstorage.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using models::storage::KwPair;
using result::Result;

class MapStorage : public KwStorage,
                   public CollectableStorage,
                   public folly::hazptr_obj_base<MapStorage> {
private:
  using Map = boost::concurrent_flat_map<std::string, std::string>;

public:
  virtual Result<KwPair> Get(std::string key) override;
  virtual Result<> Remove(std::string key) override;
  virtual void Insert(KwPair data) override;
  virtual void Clear() override;

  virtual std::vector<KwPair> Collect() override;

  Map&& MoveUnderlying();

private:
  Map map_;
};

class ReadonlyMemoryStorage {
private:
  using Map = boost::unordered::unordered_flat_map<std::string, std::string>;

public:
  ReadonlyMemoryStorage(MapStorage&& storage);

  struct Iterator {
    using difference_type = std::ptrdiff_t;
    using value_type = KwPair;

    value_type operator*() const;

    Iterator& operator++();
    Iterator operator++(int);

    bool operator==(const Iterator& other) const = default;

    Map::const_iterator map_iterator_;
  };
  static_assert(std::input_iterator<Iterator>);

  Iterator begin() const;
  Iterator end() const;

private:
  Map map_;
};

}  // namespace stewkk::db::logic::storage
