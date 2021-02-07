load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
git_repository(
    name = "gtest",
    remote = "https://github.com/google/googletest",
    branch = "v1.10.x",
)
http_archive(
    name = "com_google_absl",
    urls = ["https://github.com/abseil/abseil-cpp/archive/98eb410c93ad059f9bba1bf43f5bb916fc92a5ea.zip"],
    strip_prefix = "abseil-cpp-98eb410c93ad059f9bba1bf43f5bb916fc92a5ea",
)

http_archive(
    name = "rules_cc",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/262ebec3c2296296526740db4aefce68c80de7fa.zip"],
    strip_prefix= "rules_cc-262ebec3c2296296526740db4aefce68c80de7fa",
)

http_archive(
    name = "com_google_googletest",
    urls = ["https://github.com/google/googletest/archive/011959aafddcd30611003de96cfd8d7a7685c700.zip"],
    strip_prefix = "googletest-011959aafddcd30611003de96cfd8d7a7685c700",
)

# Google benchmark.
http_archive(
    name = "com_github_google_benchmark",
    urls = ["https://github.com/google/benchmark/archive/bf585a2789e30585b4e3ce6baf11ef2750b54677.zip"],  # 2020-11-26T11:14:03Z
    strip_prefix = "benchmark-bf585a2789e30585b4e3ce6baf11ef2750b54677",
    sha256 = "2a778d821997df7d8646c9c59b8edb9a573a6e04c534c01892a40aa524a7b68c",
)
