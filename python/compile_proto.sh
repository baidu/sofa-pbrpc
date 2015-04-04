#!/bin/sh
#
# Compile depending proto files to *_pb2.py.

. ../depends.mk

PROTOC=$PROTOBUF_DIR/bin/protoc
if [ ! -f $PROTOC ]
then
    echo "protoc is not found, please set PROTOBUF_DIR in '../depends.mk'."
    exit 1
fi

files="rpc_option.proto rpc_meta.proto builtin_service.proto"
for file in $files
do
    input="../src/sofa/pbrpc/$file"
    output="./sofa/pbrpc/${file/.proto/_pb2.py}"
    echo "Generating $output..."
    $PROTOC -I../src -I. -I$PROTOBUF_DIR/include --python_out=. $input
done

