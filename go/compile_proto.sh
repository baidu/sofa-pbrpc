#!/bin/bash

SRCROOT=$GOPATH/src/github.com/baidu/sofa-pbrpc/src

protoc -I $SRCROOT --go_out=import_path=meta:. $SRCROOT/sofa/pbrpc/rpc_meta.proto $SRCROOT/sofa/pbrpc/rpc_option.proto

mkdir -p rpc_meta
mv sofa/pbrpc/*.go rpc_meta
rm -r sofa
