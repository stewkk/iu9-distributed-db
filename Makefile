codegen:
	protol --create-package --in-place --python-out ./src/stewkk/client/codegen/ protoc --proto-path=./src/stewkk/db/ --plugin=protoc-gen-grpc_python=./build/bin/grpc_python_plugin --python_out=./src/stewkk/client/codegen/ --pyi_out=./src/stewkk/client/codegen/ --grpc_python_out=./src/stewkk/client/codegen/ src/stewkk/db/example.proto

.PHONY: codegen
