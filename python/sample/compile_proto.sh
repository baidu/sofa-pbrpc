#!/bin/sh
#
# Compile depending proto files to *_pb2.py.

. ../../depends.mk

PROTOC=$PROTOBUF_DIR/bin/protoc
if [ ! -f $PROTOC ]
then
    echo "protoc is not found, please set PROTOBUF_DIR in '../../depends.mk'."
    exit 1
fi

$PROTOC -I. --python_out=. echo_service.proto

