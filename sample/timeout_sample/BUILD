load("@protobuf//:protobuf.bzl", "cc_proto_library")

cc_proto_library(
  name = "sleep_proto",
  srcs = [
    "sleep_service.proto",
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
    ":sleep_proto",
    "//:sofa-pbrpc",
    "@protobuf//:protobuf",
    "@protobuf//:protoc_lib",
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "client",
  srcs = ["client.cc"],

  deps = [
    ":sleep_proto",
    "//:sofa-pbrpc",
    "@protobuf//:protobuf",
    "@protobuf//:protoc_lib",
  ],
  visibility = ["//main:__pkg__"],
)
