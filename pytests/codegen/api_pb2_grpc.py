"""Client and server classes corresponding to protobuf-defined services."""
import grpc
from . import api_pb2 as api__pb2

class DbStub(object):
    """Missing associated documentation comment in .proto file."""

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.Insert = channel.unary_unary('/Db/Insert', request_serializer=api__pb2.KeyValueRequest.SerializeToString, response_deserializer=api__pb2.EmptyReply.FromString, _registered_method=True)
        self.Update = channel.unary_unary('/Db/Update', request_serializer=api__pb2.KeyValueRequest.SerializeToString, response_deserializer=api__pb2.EmptyReply.FromString, _registered_method=True)
        self.Remove = channel.unary_unary('/Db/Remove', request_serializer=api__pb2.KeyRequest.SerializeToString, response_deserializer=api__pb2.EmptyReply.FromString, _registered_method=True)
        self.Get = channel.unary_unary('/Db/Get', request_serializer=api__pb2.KeyRequest.SerializeToString, response_deserializer=api__pb2.GetReply.FromString, _registered_method=True)
        self.Clear = channel.unary_unary('/Db/Clear', request_serializer=api__pb2.EmptyRequest.SerializeToString, response_deserializer=api__pb2.EmptyReply.FromString, _registered_method=True)

class DbServicer(object):
    """Missing associated documentation comment in .proto file."""

    def Insert(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Update(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Remove(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Get(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Clear(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

def add_DbServicer_to_server(servicer, server):
    rpc_method_handlers = {'Insert': grpc.unary_unary_rpc_method_handler(servicer.Insert, request_deserializer=api__pb2.KeyValueRequest.FromString, response_serializer=api__pb2.EmptyReply.SerializeToString), 'Update': grpc.unary_unary_rpc_method_handler(servicer.Update, request_deserializer=api__pb2.KeyValueRequest.FromString, response_serializer=api__pb2.EmptyReply.SerializeToString), 'Remove': grpc.unary_unary_rpc_method_handler(servicer.Remove, request_deserializer=api__pb2.KeyRequest.FromString, response_serializer=api__pb2.EmptyReply.SerializeToString), 'Get': grpc.unary_unary_rpc_method_handler(servicer.Get, request_deserializer=api__pb2.KeyRequest.FromString, response_serializer=api__pb2.GetReply.SerializeToString), 'Clear': grpc.unary_unary_rpc_method_handler(servicer.Clear, request_deserializer=api__pb2.EmptyRequest.FromString, response_serializer=api__pb2.EmptyReply.SerializeToString)}
    generic_handler = grpc.method_handlers_generic_handler('Db', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))
    server.add_registered_method_handlers('Db', rpc_method_handlers)

class Db(object):
    """Missing associated documentation comment in .proto file."""

    @staticmethod
    def Insert(request, target, options=(), channel_credentials=None, call_credentials=None, insecure=False, compression=None, wait_for_ready=None, timeout=None, metadata=None):
        return grpc.experimental.unary_unary(request, target, '/Db/Insert', api__pb2.KeyValueRequest.SerializeToString, api__pb2.EmptyReply.FromString, options, channel_credentials, insecure, call_credentials, compression, wait_for_ready, timeout, metadata, _registered_method=True)

    @staticmethod
    def Update(request, target, options=(), channel_credentials=None, call_credentials=None, insecure=False, compression=None, wait_for_ready=None, timeout=None, metadata=None):
        return grpc.experimental.unary_unary(request, target, '/Db/Update', api__pb2.KeyValueRequest.SerializeToString, api__pb2.EmptyReply.FromString, options, channel_credentials, insecure, call_credentials, compression, wait_for_ready, timeout, metadata, _registered_method=True)

    @staticmethod
    def Remove(request, target, options=(), channel_credentials=None, call_credentials=None, insecure=False, compression=None, wait_for_ready=None, timeout=None, metadata=None):
        return grpc.experimental.unary_unary(request, target, '/Db/Remove', api__pb2.KeyRequest.SerializeToString, api__pb2.EmptyReply.FromString, options, channel_credentials, insecure, call_credentials, compression, wait_for_ready, timeout, metadata, _registered_method=True)

    @staticmethod
    def Get(request, target, options=(), channel_credentials=None, call_credentials=None, insecure=False, compression=None, wait_for_ready=None, timeout=None, metadata=None):
        return grpc.experimental.unary_unary(request, target, '/Db/Get', api__pb2.KeyRequest.SerializeToString, api__pb2.GetReply.FromString, options, channel_credentials, insecure, call_credentials, compression, wait_for_ready, timeout, metadata, _registered_method=True)

    @staticmethod
    def Clear(request, target, options=(), channel_credentials=None, call_credentials=None, insecure=False, compression=None, wait_for_ready=None, timeout=None, metadata=None):
        return grpc.experimental.unary_unary(request, target, '/Db/Clear', api__pb2.EmptyRequest.SerializeToString, api__pb2.EmptyReply.FromString, options, channel_credentials, insecure, call_credentials, compression, wait_for_ready, timeout, metadata, _registered_method=True)
