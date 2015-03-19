#!/bin/sh

PROTO_INCLUDE=$1

protoc --proto_path=. --proto_path=${PROTO_INCLUDE} --proto_path=/usr/local/include --cpp_out=. \
sofa/pbrpc/rpc_meta.proto sofa/pbrpc/rpc_option.proto sofa/pbrpc/builtin_service.proto

