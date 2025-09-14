#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/controllers/dto.hpp>
#include <stewkk/db/logic/result/result.hpp>

namespace stewkk::db::logic::controllers {

class GetController {
public:
  virtual result::Result<ValueDTO> Get(const boost::asio::yield_context& yield, GetRequestDTO data)
      = 0;

  virtual ~GetController() = default;
};

}  // namespace stewkk::db::logic::controllers
