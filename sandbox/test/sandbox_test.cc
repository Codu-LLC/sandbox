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
#include <iostream>

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
                for(int i = 0; i < 10000000; i++) test += "test";
                exit(EXIT_SUCCESS);
            }
        default: /* Parent */
            waitpid(pid, NULL, 0);
            std::cout << cgroup->get_property("memory", "limit_in_bytes") << std::endl;
            std::cout << cgroup->get_property("memory", "max_usage_in_bytes") << std::endl;
            EXPECT_TRUE(std::stoll(cgroup->get_property("memory", "max_usage_in_bytes")) >= MB);
            break;
    }
}


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
                FILE *f = fopen("test_sandbox_file.txt", "w");
                for (int i = 0; i < (MB >> 1); i++) {
                    if (fprintf(f, "test\n") < 0) {
                        if (f != NULL) {
                            fclose(f);
                        }
                        raise(SIGXFSZ);
                        exit(EXIT_FAILURE);
                    }
                }
                if (f != NULL) {
                    fclose(f);
                }
                break;
            }
        default:
            validate_signal(pid, SIGXFSZ);
            break;
    }
}

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