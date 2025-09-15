#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/dto/dto.hpp>

namespace stewkk::db::logic::controllers {

class InsertController {
public:
  virtual result::Result<> Insert(const boost::asio::yield_context& yield, models::dto::KwDTO data)
      = 0;

  virtual ~InsertController() = default;
};

}  // namespace stewkk::db::logic::controllers
