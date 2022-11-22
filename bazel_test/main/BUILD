cc_library(
    name = "lib",
    srcs = ["lib/lib.cc"],
    hdrs = ["lib/lib.h"],
)

cc_binary(
    name = "main.out",
    srcs = ["main.cc"],
    deps = [
        ":lib",
    ],
)