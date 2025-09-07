test: build
	./build/bin/unittests

gdb: build
	gdb --args ./build/bin/unittests --gtest_break_on_failure

build:
	cmake --build build -- -j 8
	cp ./build/bin/iu9-db ./build/bin/iu9-db-fixed
	patchelf --set-interpreter "/lib64/ld-linux-x86-64.so.2" ./build/bin/iu9-db-fixed
	cp ./build/bin/iu9-db-fixed ./jepsen/iu9-distributed-db/resources/iu9-db

codegen:
	protol --create-package --in-place --python-out ./src/stewkk/client/codegen/ protoc --proto-path=./src/stewkk/db/ --plugin=protoc-gen-grpc_python=./build/bin/grpc_python_plugin --python_out=./src/stewkk/client/codegen/ --pyi_out=./src/stewkk/client/codegen/ --grpc_python_out=./src/stewkk/client/codegen/ src/stewkk/db/api.proto
	./scripts/codegen-java-client.sh

jepsen:
	make --directory jepsen/iu9-distributed-db

.PHONY: codegen test build jepsen
