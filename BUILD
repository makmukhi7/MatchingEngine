cc_library(
    name = "messages",
    hdrs = ["messages.h"],
    srcs = ["messages.cc"],
)

cc_test(
    name = "messages_test",
    size = "small",
    srcs = ["messages_test.cc"],
    deps = [
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "//:messages",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = ["//:messages"],
)