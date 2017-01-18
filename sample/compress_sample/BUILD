load("@protobuf//:protobuf.bzl", "cc_proto_library")

config_setting(
  name = "darwin",
  values = {
    "cpu": "darwin",
  },
  visibility = ["//visibility:public"],
)

cc_proto_library(
  name = "echo_proto",
  srcs = [
    "echo_service.proto",
  ],
  include = ".",
  default_runtime = "@protobuf//:protobuf",
  protoc = "@protobuf//:protoc",
  deps = [
    "//:cc_protos",
  ],
)

cc_binary(
  name = "server",
  srcs = ["server.cc"],

  deps = [
    ":echo_proto",
    "//:sofa-pbrpc",
    "@protobuf//:protobuf",
    "@protobuf//:protoc_lib",
  ],
  linkopts = select({
    ":darwin": [],
    "//conditions:default": ["-static"]
  }),
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "client",
  srcs = ["client.cc"],

  deps = [
    ":echo_proto",
    "//:sofa-pbrpc",
    "@protobuf//:protobuf",
    "@protobuf//:protoc_lib",
  ],
  linkopts = select({
    ":darwin": [],
    "//conditions:default": ["-static"]
  }),
  visibility = ["//main:__pkg__"],
)
