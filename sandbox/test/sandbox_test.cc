//
// Created by Eugene Junghyun Kim on 1/10/2021.
//

#include "sandbox/sandbox.h"
#include "sandbox/sandbox_builder.h"
#include "gtest/gtest.h"
#include <csignal>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <unistd.h>

#define FAIL_TEST EXPECT_TRUE(false)

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

TEST(SandboxTest, CheckAddressSpaceLimit) {
    auto pid = fork();
    switch(pid) {
        case -1:
            // Fork failed.
            FAIL_TEST;
            break;
        case 0: /* Child */
            {
                set_sandbox_limit(&sandbox);
                // Try to allocate 100MB of space
                char *test = (char *) malloc(sizeof(char) * 100 * MB);
                if (test != nullptr) {
                    free(test);
                } else {
                    // Raise SIGSEGV to let parent know that memory allocation failed.
                    raise(SIGSEGV);
                }
                break;
            }
        default: /* Parent */
            validate_signal(pid, SIGSEGV);
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
        case 0: /* Child */
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
