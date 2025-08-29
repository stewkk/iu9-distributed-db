#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/controllers/dto.hpp>
#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::controllers {

class RemoveController {
public:
  virtual result::Result<> Remove(const boost::asio::yield_context& yield, KeyDTO data) = 0;

  virtual ~RemoveController() = default;
};

}  // namespace stewkk::db::logic::controllers
