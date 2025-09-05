test: build
	./build/bin/unittests

gdb: build
	gdb --args ./build/bin/unittests --gtest_break_on_failure

build:
	cmake --build build -- -j 8
	cp ./build/bin/iu9-db ./build/bin/iu9-db-fixed
	patchelf --set-interpreter "/lib64/ld-linux-x86-64.so.2" ./build/bin/iu9-db-fixed

codegen:
	protol --create-package --in-place --python-out ./pytests/codegen/ protoc --proto-path=./src/stewkk/db/ --plugin=protoc-gen-grpc_python=./build/bin/grpc_python_plugin --python_out=./pytests/codegen/ --pyi_out=./pytests/codegen/ --grpc_python_out=./pytests/codegen/ src/stewkk/db/api.proto

testsuite:
	echo "not implemented"
	# PYTHONPATH=./pytests pytest -vvv pytests/ --app-service-port 50051

.PHONY: codegen test build
