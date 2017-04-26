# sofa-pbrpc-go

Go版本sofa-pbrpc的客户端和服务端实现。


# 安装

`$ go get github.com/baidu/sofa-pbrpc/go`

# 依赖

go1.7+

# Quick Start

```
$ go get -u github.com/baidu/sofa-pbrpc/go/examples/echo/echo_server
$ go get -u github.com/baidu/sofa-pbrpc/go/examples/echo/echo_client
$ echo_server &
$ echo_client
```

# 生成示例代码

1. 安装 protoc
2. 安装protoc插件

```
$ go get -u github.com/baidu/sofa-pbrpc/go/protoc-gen-go
$ cd $GOPATH/github.com/baidu/sofa-pbrpc/go/examples/echo
$ ./compile_proto.sh
```

# 功能说明

- 对压缩的支持是受限的，目前客户端和服务端可以解析来自于c++的压缩请求或返回，但go没有明确的地方可以主动发起压缩请求
