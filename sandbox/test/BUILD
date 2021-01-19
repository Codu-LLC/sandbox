cc_test(
    name = "test",
    srcs = glob(["*_test.cc"]),
    linkopts = [
        "-lcap",
    ],
    deps = [
        "//sandbox",
        "@gtest//:gtest",
        "@gtest//:gtest_main",
    ]
)
