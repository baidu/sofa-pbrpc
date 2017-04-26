#!/bin/bash

SRCROOT=$GOPATH/src/github.com/baidu/sofa-pbrpc

protoc -I $SRCROOT/sample --go_out=import_path=echo,plugins=pbrpc:. $SRCROOT/sample/echo/echo_service.proto
