load("@protobuf//:protobuf.bzl", "cc_proto_library")

RELATIVE_WELL_KNOWN_PROTOS = [
  "sofa/pbrpc/builtin_service.proto",
  "sofa/pbrpc/rpc_meta.proto",
  "sofa/pbrpc/rpc_option.proto"
]

WELL_KNOWN_PROTOS = ["src/" + s for s in RELATIVE_WELL_KNOWN_PROTOS]

filegroup(
  name = "well_known_protos",
  srcs = WELL_KNOWN_PROTOS,
  visibility = ["//visibility:public"],
)

cc_proto_library(
  name = "cc_protos",
  srcs = WELL_KNOWN_PROTOS,
  include = "src/",
  default_runtime = "@protobuf//:protobuf",
  protoc = "@protobuf//:protoc",
  deps = ["@protobuf//:cc_wkt_protos"],
  visibility = ["//visibility:public"]
)

cc_library(
  name = "rapidjson",
  hdrs = glob(["src/rapidjson/*.h", "src/rapidjson/internal/*.h"]),
  srcs = [],
  visibility = ["//main:__pkg__"],
)

cc_library(
  name = "sofa-pbrpc",
  hdrs = glob([
    "src/sofa/pbrpc/*.h",
    "src/sofa/pbrpc/smart_ptr/*.hpp",
    "src/sofa/pbrpc/smart_ptr/detail/*.hpp"
  ]),
  srcs = glob(["src/sofa/pbrpc/*.cc"]),
  deps = [
    ":cc_protos",
    ":rapidjson",
    "@protobuf//:protobuf",
    "@protobuf//:protoc_lib",
    "@snappy//:snappy",
    "@zlib//:zlib",
    "@gtest//:gtest",
    "@boost//:boost"
  ],
  copts = [
    "-Iexternal/googletest/include",
  ],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "sofa-pbrpc-client-lib",
  hdrs = ["src/sofa/pbrpc/http-agent/http_agent.h"],
  srcs = ["src/sofa/pbrpc/http-agent/http_agent.cc"],
  deps = [
    ":sofa-pbrpc",
  ],
  visibility = ["//main:__pkg__"],
)

cc_binary(
  name = "sofa-pbrpc-client",
  srcs = [
    "src/sofa/pbrpc/http-agent/sofa_pbrpc_client.cc",
  ],
  deps = [
    ":sofa-pbrpc-client-lib",
  ],
  visibility = ["//main:__pkg__"],
)

exports_files(["src/sofa/pbrpc/buffer.cc"], ["//unit-test:__pkg__"])
