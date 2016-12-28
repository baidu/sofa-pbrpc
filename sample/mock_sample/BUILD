load("@protobuf//:protobuf.bzl", "cc_proto_library")

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
  name = "test",
  srcs = ["mock_test_sample.cc"],

  deps = [
    ":echo_proto",
    "//:sofa-pbrpc",
    "@gtest//:gtest",
  ],
  visibility = ["//main:__pkg__"],
)
