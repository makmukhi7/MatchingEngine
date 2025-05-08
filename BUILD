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

cc_library(
    name = "order_book",
    hdrs = ["order_book.h"],
    srcs = ["order_book.cc"],
    deps = [":messages"],
)

cc_test(
    name = "order_book_test",
    size = "small",
    srcs = ["order_book_test.cc"],
    deps = [
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "//:order_book",
    ],
)

cc_library(
    name = "matching_engine",
    hdrs = ["matching_engine.h"],
    srcs = ["matching_engine.cc"],
    deps = [
        ":messages",
        ":order_book",
    ],
)

cc_test(
    name = "matching_engine_test",
    size = "small",
    srcs = ["matching_engine_test.cc"],
    data = glob(["testdata/**"]),
    deps = [
        "@googletest//:gtest",
        "@googletest//:gtest_main",
        "//:matching_engine",

    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = ["//:matching_engine"],
)