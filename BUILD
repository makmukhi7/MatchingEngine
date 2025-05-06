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

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = ["//:messages"],
)