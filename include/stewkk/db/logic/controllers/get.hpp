#pragma once

#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/dto/dto.hpp>

namespace stewkk::db::logic::controllers {

class GetController {
public:
  virtual result::Result<models::dto::ValueDTO> Get(const boost::asio::yield_context& yield,
                                                    models::dto::GetRequestDTO data)
      = 0;

  virtual ~GetController() = default;
};

}  // namespace stewkk::db::logic::controllers
