#pragma once

#include <string>

#include <boost/unordered/concurrent_flat_map.hpp>
#include <boost/unordered/unordered_flat_map.hpp>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::storage {

using models::storage::KwPair;
using result::Result;

class MemoryStorage {
private:
  using Map = boost::concurrent_flat_map<std::string, std::string>;

public:
  Result<KwPair> Get(std::string key);
  Result<> Remove(std::string key);
  void Insert(KwPair data);
  Result<> Update(KwPair data);

  Map&& GetUnderlying() &&;

private:
  Map map_;
};

class ReadonlyMemoryStorage {
private:
  using Map = boost::unordered::unordered_flat_map<std::string, std::string>;

public:
  ReadonlyMemoryStorage(MemoryStorage&& storage);

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
