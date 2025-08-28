#!/usr/bin/env python3

import grpc
import codegen.api_pb2 as api_pb2
import codegen.api_pb2_grpc as api_pb2_grpc
from grpc_health.v1 import health_pb2
from grpc_health.v1 import health_pb2_grpc


class Client:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.channel = grpc.insecure_channel(self.__target())
        self.stub = api_pb2_grpc.DbStub(self.channel)
        self.health_stub = health_pb2_grpc.HealthStub(self.channel)


    def __del__(self):
        self.channel.close()

    def __target(self):
        return "{}:{}".format(self.host, self.port)

    def Get(self, key):
        try:
            response = self.stub.Get(api_pb2.KeyRequest(key=key))
            return response.value
        except grpc.RpcError as rpc_error:
            return (rpc_error.code(), rpc_error.details())

    def Remove(self, key):
        try:
            response = self.stub.Remove(api_pb2.KeyRequest(key=key))
            return None
        except grpc.RpcError as rpc_error:
            return (rpc_error.code(), rpc_error.details())

    def Insert(self, key, value):
        try:
            response = self.stub.Insert(api_pb2.KeyValueRequest(key=key, value=value))
            return None
        except grpc.RpcError as rpc_error:
            return (rpc_error.code(), rpc_error.details())

    def Update(self, key, value):
        try:
            response = self.stub.Update(api_pb2.KeyValueRequest(key=key, value=value))
            return None
        except grpc.RpcError as rpc_error:
            return (rpc_error.code(), rpc_error.details())

    def Clear(self):
        try:
            response = self.stub.Clear(api_pb2.EmptyRequest())
            return None
        except grpc.RpcError as rpc_error:
            return (rpc_error.code(), rpc_error.details())

    async def Healthcheck(self, session=None, process=None):
        request = health_pb2.HealthCheckRequest()
        resp = None
        try:
            resp = self.health_stub.Check(request)
        except grpc.RpcError as rpc_error:
            return False
        if resp.status == health_pb2.HealthCheckResponse.SERVING:
            return True
        elif resp.status == health_pb2.HealthCheckResponse.NOT_SERVING:
            return False
        return False
