cc_library(
  name = "gtest",
  hdrs = glob([
    "googletest/include/**/*.h",
    "googletest/src/*.h"
  ]),
  srcs = glob(
    ["googletest/src/*.cc"],
    exclude = ["googletest/src/gtest-all.cc"]
  ),
  includes = ["googletest/include", "googletest"],
  copts = ["-Iexternal/googletest/include"],
  linkopts = ["-pthread"],
  visibility = ["//visibility:public"],
)
