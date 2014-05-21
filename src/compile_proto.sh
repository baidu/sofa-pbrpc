#!/bin/sh

PROTOC=$PROTOBUF_DIR/bin/protoc

$PROTOC --proto_path=. --cpp_out=. sofa/pbrpc/rpc_meta.proto sofa/pbrpc/rpc_option.proto sofa/pbrpc/builtin_service.proto

