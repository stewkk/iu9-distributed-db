#!/usr/bin/env sh

protoc --plugin=protoc-gen-grpc_java=$(which protoc-gen-grpc-java) --java_out=./jepsen/iu9-distributed-db/target/generated-sources/protobuf --grpc_java_out=./jepsen/iu9-distributed-db/target/generated-sources/protobuf src/stewkk/db/api.proto
