load("@protobuf//:protobuf.bzl", "cc_proto_library")

cc_proto_library(
  name = "test_data",
  srcs = [
    "test_data.proto",
  ],
  include = ".",
  default_runtime = "@protobuf//:protobuf",
  protoc = "@protobuf//:protoc",
  deps = [
    "//:cc_protos",
  ],
)

cc_binary(
  name = "test_atomic",
  srcs = ["test_atomic.cc"],
  deps = [
    "//:sofa-pbrpc",
  ],
  visibility = ["//main:__pkg__"],
)

cc_library(
  name = "test_buffer-deps",
  hdrs = [
    "//:src/sofa/pbrpc/buffer.cc",
  ],
  deps = [
    "//:sofa-pbrpc"
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "test_buffer",
  srcs = ["test_buffer.cc"],
  deps = [
    ":test_data",
    "//:sofa-pbrpc",
    ":test_buffer-deps",
  ],
  includes = ["src"],
  copts = [
    "-fno-access-control",
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "test_closure",
  srcs = ["test_closure.cc"],
  deps = [
    "//:sofa-pbrpc"
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "test_common",
  srcs = ["test_common.cc"],
  deps = [
    "//:sofa-pbrpc"
  ],
  copts = [
    "-DBAZEL_BUILD"
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "test_epoll_support",
  srcs = ["test_epoll_support.cc"],
  deps = [
    "//:sofa-pbrpc"
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "test_ext_closure",
  srcs = ["test_ext_closure.cc"],
  deps = [
    "//:sofa-pbrpc"
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "test_io_service_pool",
  srcs = ["test_io_service_pool.cc"],
  deps = [
    "//:sofa-pbrpc"
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "test_thread_group",
  srcs = ["test_thread_group.cc"],
  deps = [
    "//:sofa-pbrpc"
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "test_timeout_manager",
  srcs = ["test_timeout_manager.cc"],
  deps = [
    "//:sofa-pbrpc"
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "test_tran_buf_pool",
  srcs = ["test_tran_buf_pool.cc"],
  deps = [
    "//:sofa-pbrpc"
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "test_web_service",
  srcs = ["test_web_service.cc"],
  deps = [
    "//:sofa-pbrpc"
  ],
  copts = [
    "-fno-access-control",
  ],
  visibility = ["//main:__pkg__"],
)
