#!/usr/bin/env python3

import logging

import grpc
import codegen.example_pb2 as example_pb2
import codegen.example_pb2_grpc as example_pb2_grpc


def run():
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.
    print("Will try to greet world ...")
    with grpc.insecure_channel("localhost:50051") as channel:
        stub = example_pb2_grpc.GreeterStub(channel)
        response = stub.SayHello(example_pb2.HelloRequest(name="you"))
    print("Greeter client received: " + response.message)


if __name__ == "__main__":
    logging.basicConfig()
    run()
