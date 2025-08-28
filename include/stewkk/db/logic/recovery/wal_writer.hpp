#pragma once

#include <string>

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/storage/kw_pair.hpp>

namespace stewkk::db::logic::recovery {

using models::storage::KwPair;
using result::Result;

class WALWriter {
public:
  virtual Result<> Remove(const boost::asio::yield_context& yield, std::string key) = 0;
  virtual Result<> Insert(const boost::asio::yield_context& yield, KwPair data) = 0;
  virtual Result<> Update(const boost::asio::yield_context& yield, KwPair data) = 0;

  virtual ~WALWriter() = default;
};

}  // namespace stewkk::db::logic::recovery
