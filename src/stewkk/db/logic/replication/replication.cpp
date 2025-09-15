#include <stewkk/db/logic/replication/replication.hpp>

#include <absl/log/log.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/generic/generic_stub.h>
#include <agrpc/client_rpc.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <stewkk/db/logic/coordination/zookeeper.hpp>

namespace stewkk::db::logic::replication {

namespace {

template <typename Message> grpc::ByteBuffer serialize(const Message& message) {
  grpc::ByteBuffer buffer;
  bool own_buffer;
  grpc::GenericSerialize<grpc::ProtoBufferWriter, Message>(message, &buffer, &own_buffer);
  return buffer;
}

template <class Message> bool deserialize(grpc::ByteBuffer& buffer, Message& message) {
  return grpc::GenericDeserialize<grpc::ProtoBufferReader, Message>(&buffer, &message).ok();
}

enum class RequestType {
  kGet,
  kInsert,
  kRemove,
};

std::string ToString(RequestType type) {
  switch (type) {
    case RequestType::kGet:
      return "/iu9db.Db/Get";
    case RequestType::kInsert:
      return "/iu9db.Db/Insert";
    case RequestType::kRemove:
      return "/iu9db.Db/Remove";
  }
}

template <RequestType type, typename Request, typename Response>
result::Result<Response> MakeRequest(const boost::asio::yield_context& yield, Request request,
                                     agrpc::GrpcContext& grpc_context, grpc::GenericStub& stub) {
  using RPC = agrpc::GenericUnaryClientRPC;

  grpc::ClientContext client_context;
  client_context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));

  auto request_buffer = serialize(request);

  grpc::ByteBuffer response_buffer;
  auto status = RPC::request(grpc_context, ToString(type), stub, client_context, request_buffer,
                             response_buffer, yield);
  if (!status.ok()) {
    return result::MakeError("failed when requesting replica: {}: {}", status.error_message(),
                             status.error_details());
  }

  Response response;
  if (!deserialize(response_buffer, response)) {
    return result::MakeError("failed when requesting replica: {}: {}", status.error_message(),
                             status.error_details());
  }

  return response;
}

template <RequestType type, typename Request, typename Response>
result::Result<Response> MakeRequestWithBackoff(const boost::asio::yield_context& yield,
                                                Request request, agrpc::GrpcContext& grpc_context,
                                                grpc::GenericStub& stub) {
  int delay = 1;
  for (int i = 0; i < 5; i++) {
    auto got = MakeRequest<type, Request, Response>(yield, request, grpc_context, stub);
    if (got.has_value()) {
      return got;
    }
    boost::asio::deadline_timer timer{grpc_context};
    timer.expires_from_now(boost::posix_time::seconds(delay));
    timer.async_wait(yield);
    delay *= 2;
  }
}

}  // namespace

ReplicaClient::ReplicaClient(std::string host, agrpc::GrpcContext& grpc_context)
    : stub_{grpc::CreateChannel(host, grpc::InsecureChannelCredentials())},
      grpc_context_(grpc_context) {}

result::Result<models::dto::ValueDTO> ReplicaClient::Get(const boost::asio::yield_context& yield,
                                                         models::dto::GetRequestDTO data) {
  iu9db::GetRequest request;
  request.set_key(data.key);
  request.set_source(iu9db::Source::SOURCE_NODE);

  auto res = MakeRequestWithBackoff<RequestType::kGet, iu9db::GetRequest, iu9db::GetReply>(
      yield, request, grpc_context_, stub_);
  if (res.has_failure()) {
    return result::WrapError(std::move(res), "get");
  }
  auto reply = std::move(res).assume_value();

  models::dto::ValueDTO dto;
  dto.value = reply.value();
  dto.version = reply.version();
  return dto;
}

result::Result<> ReplicaClient::Insert(const boost::asio::yield_context& yield,
                                       models::dto::KwDTO data) {
  iu9db::KeyValueRequest request;
  request.set_key(data.key);
  request.set_value(data.value);
  request.set_version(data.version.value());
  request.set_source(iu9db::Source::SOURCE_NODE);

  auto res
      = MakeRequestWithBackoff<RequestType::kInsert, iu9db::KeyValueRequest, iu9db::EmptyReply>(
          yield, request, grpc_context_, stub_);
  if (res.has_failure()) {
    return result::WrapError(std::move(res), "insert");
  }
  return result::Ok();
}

result::Result<> ReplicaClient::Remove(const boost::asio::yield_context& yield,
                                       models::dto::KeyDTO data) {
  iu9db::KeyRequest request;
  request.set_key(data.key);
  request.set_version(data.version.value());
  request.set_source(iu9db::Source::SOURCE_NODE);

  auto res = MakeRequestWithBackoff<RequestType::kRemove, iu9db::KeyRequest, iu9db::EmptyReply>(
      yield, request, grpc_context_, stub_);
  if (res.has_failure()) {
    return result::WrapError(std::move(res), "remove");
  }
  return result::Ok();
}

Replication::Replication(agrpc::GrpcContext& grpc_context) : grpc_context_(grpc_context) {}

result::Result<> Replication::SendInsertToReplicas(const boost::asio::yield_context& yield,
                                                   models::dto::KwDTO data) {
  auto nodes_opt = coordination::GetNodes();
  if (nodes_opt.has_failure()) {
    return result::WrapError(std::move(nodes_opt), "failed to get nodes list");
  }
  auto nodes = std::move(nodes_opt).assume_value();

  for (auto& node : nodes) {
    result::Result<> err = result::Ok();
    auto do_request = [&](auto& value) { err = value.second.Insert(yield, data); };
    if (host_to_client_.emplace_or_visit(node, ReplicaClient(node, grpc_context_), do_request)) {
      host_to_client_.visit(node, do_request);
    }
    if (err.has_failure()) {
      LOG(WARNING) << "error while sending insert request to replicas: "
                   << err.assume_error().What();
    }
    LOG(INFO) << "replicated insert request with key=" << data.key << " to node=" << node;
  }
  return result::Ok();
}

result::Result<> Replication::SendRemoveToReplicas(const boost::asio::yield_context& yield,
                                                   models::dto::KeyDTO data) {
  auto nodes_opt = coordination::GetNodes();
  if (nodes_opt.has_failure()) {
    return result::WrapError(std::move(nodes_opt), "failed to get nodes list");
  }
  auto nodes = std::move(nodes_opt).assume_value();

  // TODO: make it concurrent tasks
  for (auto& node : nodes) {
    result::Result<> err = result::Ok();
    auto do_request = [&](auto& value) { err = value.second.Remove(yield, data); };
    if (host_to_client_.emplace_or_visit(node, ReplicaClient(node, grpc_context_), do_request)) {
      host_to_client_.visit(node, do_request);
    }
    if (err.has_failure()) {
      LOG(WARNING) << "error while sending remove request to replicas: "
                   << err.assume_error().What();
    }
    LOG(INFO) << "replicated remove request with key=" << data.key << " to node=" << node;
  }
  return result::Ok();
}

result::Result<models::dto::ValueDTO> Replication::GetNewestFromReplicas(
    const boost::asio::yield_context& yield, models::dto::GetRequestDTO key) {
  auto nodes_opt = coordination::GetNodes();
  if (nodes_opt.has_failure()) {
    return result::WrapError(std::move(nodes_opt), "failed to get nodes list");
  }
  auto nodes = std::move(nodes_opt).assume_value();

  result::Result<models::dto::ValueDTO> result
      = result::MakeError("failed to retrieve value from replicas");
  for (auto& node : nodes) {
    result::Result<models::dto::ValueDTO> err = result::Ok(models::dto::ValueDTO{});
    auto do_request = [&](auto& value) { err = value.second.Get(yield, key); };
    if (host_to_client_.emplace_or_visit(node, ReplicaClient(node, grpc_context_), do_request)) {
      host_to_client_.visit(node, do_request);
    }

    if (err.has_failure()) {
      continue;
    }
    if (!result.has_value() || result.assume_value().version < err.assume_value().version) {
      result = err.assume_value();
    }
  }
  return result;
}

}  // namespace stewkk::db::logic::replication
