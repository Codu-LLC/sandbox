//
// Created by Eugene Junghyun Kim on 1/10/2021.
//

#include "sandbox/sandbox.h"
#include "sandbox/sandbox_builder.h"
#include "gtest/gtest.h"
#include <string>

TEST(SandboxBuilderTest, ProducesCorrectSandboxConfiguration) {
    auto sandbox = Sandbox::builder()
                .set_debug(true)
                .set_time_limit(1000)
                .set_memory_limit(32)
                .set_sandbox_dir("/sandbox")
                .set_src_root_fs_dir("/src/rootfs")
                .set_target_root_fs_dir("/target/rootfs")
                .set_command("./sandbox/test")
                .build();
    EXPECT_EQ(sandbox.get_time_limit_in_ms(), 1000);
    EXPECT_EQ(sandbox.get_memory_limit_in_mb(), 32);
    EXPECT_EQ(sandbox.get_sandbox_dir(), "/sandbox");
    EXPECT_EQ(sandbox.get_src_root_fs_dir(), "/src/rootfs");
    EXPECT_EQ(sandbox.get_target_root_fs_dir(), "/target/rootfs");
    EXPECT_EQ(sandbox.get_command(), "./sandbox/test");
}