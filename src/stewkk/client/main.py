#!/usr/bin/env python3

import logging

import grpc
import codegen.api_pb2 as api_pb2
import codegen.api_pb2_grpc as api_pb2_grpc


def run():
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.
    print("Requesting db ...")
    with grpc.insecure_channel("localhost:50051") as channel:
        stub = api_pb2_grpc.DbStub(channel)
        response = stub.Get(api_pb2.KeyRequest(key="oaoa"))
    print("Received: " + response.value)


if __name__ == "__main__":
    logging.basicConfig()
    run()
