#pragma once

#include <boost/outcome.hpp>
#include <boost/outcome/success_failure.hpp>

#include <expected>
#include <string>
#include <unordered_map>

namespace stewkk::db::logic::storage {

struct KwPair {
  std::string key;
  std::string value;

  bool operator==(const KwPair& other) const = default;
};

struct EmptyResult {};

template <typename T = EmptyResult> using Result = boost::outcome_v2::result<T, std::exception_ptr>;

template <typename T = EmptyResult> auto Ok(T&& v) { return boost::outcome_v2::success(v); }

std::exception_ptr Error(const std::exception& e);

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
