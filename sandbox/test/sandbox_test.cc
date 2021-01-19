//
// Created by Eugene Junghyun Kim on 1/10/2021.
//

#include "sandbox/limit.h"
#include "sandbox/sandbox.h"
#include "sandbox/sandbox_builder.h"
#include "gtest/gtest.h"
#include <csignal>
#include <cstdlib>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unistd.h>

#define FAIL_TEST EXPECT_TRUE(false)
#define PASS_TEST EXPECT_TRUE(true)

#define MB (1 << 20)

auto sandbox = Sandbox::builder()
        .set_debug(true)
        .set_time_limit(1000) /* 1 second */
        .set_memory_limit(16) /* 16MB */
        .set_file_size_limit_in_mb(1) /* 1MB */
        .set_sandbox_dir("/sandbox")
        .set_src_root_fs_dir("/src/rootfs")
        .set_target_root_fs_dir("/target/rootfs")
        .set_command("./sandbox/test")
        .build();

static void validate_signal(pid_t pid, int expected_signal) {
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        // wait pid failed.
        FAIL_TEST;
    }
    EXPECT_TRUE(WIFSIGNALED(status));
    EXPECT_EQ(WTERMSIG(status), expected_signal);
}

// Since we cannot create cgroup from non-root user, we must run the following commands from root to set up test
// environment for this test.
// sudo cgcreate -a ${USER}:${GROUP} -g cpu,memory:/test_sandbox
// sudo chown ${USER}:${GROUP} /sys/fs/cgroup/memory/test_sandbox/tasks
// sudo chown ${USER}:${GROUP} /sys/fs/cgroup/cpu/test_sandbox/tasks
// where ${USER} and ${GROUP} is user and group for the current shell session.
TEST(SandboxTest, CheckMemoryLimit) {
    auto cgroup = setup_cgroup(&sandbox, "test_sandbox", false);
    auto pid = fork();
    switch(pid) {
        case -1:
            // Fork failed.
            FAIL_TEST;
            break;
        case 0: /* Child */
            {
                cgroup->attach_process(getpid());
                std::string test = "";
                while(1) test += "test";
                exit(EXIT_SUCCESS);
            }
        default: /* Parent */
            waitpid(pid, NULL, 0);
            EXPECT_TRUE(std::stoll(cgroup->get_property("memory", "max_usage_in_bytes")) >= MB);
            PASS_TEST;
            break;
    }
}

/*
TEST(SandboxTest, CheckMaximumFileSize) {
    auto pid = fork();
    switch(pid) {
        case -1:
            // Fork failed.
            FAIL_TEST;
            break;
        case 0:
            {
                set_sandbox_limit(&sandbox);
                // Make 2MB of output.
                for (int i = 0; i < (MB >> 1); i++) {
                    if (printf("test\n") < 0) {
                        raise(SIGXFSZ);
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            }
        default:
            validate_signal(pid, SIGXFSZ);
            break;
    }
}*/

TEST(SandboxTest, CheckMaximumSubProcesses) {
    auto pid = fork();
    switch(pid) {
        case -1:
            // Fork failed.
            FAIL_TEST;
            break;
        case 0: /* Child */
            {
                set_sandbox_limit(&sandbox);
                // This fork should fail.
                if (fork() == -1) {
                    raise(SIGINT);
                    exit(EXIT_FAILURE);
                }
                break;
            }
        default:
            validate_signal(pid, SIGINT);
        break;
    }
}