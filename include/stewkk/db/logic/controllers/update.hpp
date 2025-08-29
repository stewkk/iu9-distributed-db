#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/controllers/dto.hpp>
#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::controllers {

class UpdateController {
public:
  virtual result::Result<> Update(const boost::asio::yield_context& yield, KwDTO data) = 0;

  virtual ~UpdateController() = default;
};

}  // namespace stewkk::db::logic::controllers
