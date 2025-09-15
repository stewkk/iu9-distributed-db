#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/dto/dto.hpp>

namespace stewkk::db::logic::controllers {

class RemoveController {
public:
  virtual result::Result<> Remove(const boost::asio::yield_context& yield, models::dto::KeyDTO data)
      = 0;

  virtual ~RemoveController() = default;
};

}  // namespace stewkk::db::logic::controllers
