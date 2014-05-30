#!/bin/sh

PROTOC=$PROTOBUF_DIR/bin/protoc
PROTO_INCLUDE=$PROTOBUF_DIR/include

$PROTOC --proto_path=. --proto_path=$PROTO_INCLUDE --cpp_out=. \
sofa/pbrpc/rpc_meta.proto sofa/pbrpc/rpc_option.proto sofa/pbrpc/builtin_service.proto

