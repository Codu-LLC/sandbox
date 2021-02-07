cc_library(
    name = "sandbox",
    hdrs = glob(["*.h"]),
    srcs = glob(["*.cc"], exclude=["main.cc"]),
    visibility = ["//sandbox:__subpackages__",]
)

# Run with unshare -m -r ./bazel-bin/sandbox/app
cc_binary(
    name = "app",
    srcs = ["main.cc"],
    deps = [
        ":sandbox",
	"@com_google_absl//absl/flags:flag",
	"@com_google_absl//absl/flags:parse",
    ],
    visibility = ["//visibility:public"],
)
