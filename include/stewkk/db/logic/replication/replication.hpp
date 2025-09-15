#pragma once

#include <agrpc/client_rpc.hpp>
#include <boost/asio/spawn.hpp>

#include <stewkk/db/logic/result/result.hpp>
#include <stewkk/db/models/dto/dto.hpp>

#include <api.grpc.pb.h>

namespace stewkk::db::logic::replication {

class ReplicaClient {
public:
  ReplicaClient(std::string host, agrpc::GrpcContext& grpc_context);
  result::Result<models::dto::ValueDTO> Get(const boost::asio::yield_context& yield,
                                            models::dto::GetRequestDTO data);
  result::Result<> Insert(const boost::asio::yield_context& yield, models::dto::KwDTO data);
  result::Result<> Remove(const boost::asio::yield_context& yield, models::dto::KeyDTO data);

private:
  grpc::GenericStub stub_;
  agrpc::GrpcContext& grpc_context_;
};

}  // namespace stewkk::db::logic::replication
