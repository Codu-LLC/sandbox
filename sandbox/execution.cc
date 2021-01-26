//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#include "cgroup.h"
#include "debug.h"
#include "execution.h"
#include "file.h"
#include "limit.h"
#include <memory>
#include <sched.h>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>

#define MB_TO_BYTES(x) ((x) << 20)

static void run_child(Sandbox *ptr, int *fd) {
    // Close read file descriptor.
    if (close(fd[0]) == -1) {
        exit(EXIT_FAILURE);
    }
    if (unshare(CLONE_NEWUSER) == -1) {
        exit(EXIT_FAILURE);
    }
    print_string(ptr->is_debug(), "After unshare");
    debug_process(ptr->is_debug());
    // Pipe stdout to fd[1].
    dup2(fd[1], STDOUT_FILENO);
    char *argv[] = {NULL};
    char *env_variable[] = {NULL};
    set_sandbox_limit(ptr);
    if (execve(ptr->get_command().c_str(), argv, env_variable) != 0) {
        std::cerr << "execve error" << std::endl;
        perror("error: ");
    }
    // Close write file descriptor.
    if (close(fd[1]) == -1) {
        exit(EXIT_FAILURE);
    }
}


static int run_parent(Sandbox *ptr, std::unique_ptr<Cgroup> cgroup, pid_t pid, int *fd) {
    int status = 0;
    bool has_system_error = false, has_presentation_error = false;
    std::string output;
    if (close(fd[1]) == -1) {
        has_system_error = true;
    }

    auto pipe_limit = ptr->get_file_size_limit_in_mb() << 20;

    while (true) {
        auto pipe_read_result = File::read_pipe(fd[0], output, pipe_limit);
        switch (pipe_read_result) {
            case -1:
                has_system_error = true;
                break;
            case 0:
                break;
            case 1:
                // Presentation Error.
                has_presentation_error = true;
                break;
            default:
                break;
        }
        if (has_presentation_error) {
            kill(pid, SIGXFSZ);
            break;
        } else if (has_system_error) {
            kill(pid, SIGTERM);
            break;
        } else if (waitpid(pid, &status, WNOHANG) != 0) {
            break;
        }
    }
    waitpid(pid, &status, 0);
    // TODO(conankun): In case the process has not terminated, send SIGKILL.
    // Store output.
    ptr->set_output(std::move(output));
    // Set return code.
    ptr->set_return_code(status);
    // TODO(conankun): Add a signal catch to diversify verdict.
    debug_sandbox_stat(ptr);
    // Set time elapsed and memory usage.
    // TODO(conankun): Add safety check for std::stoll.
    ptr->set_time_elapsed(std::stoll(cgroup->get_property("cpuacct", "usage")));
    ptr->set_memory_used(std::stoll(cgroup->get_property("memory", "max_usage_in_bytes")));
    cgroup->delete_cgroup();
    return has_system_error ? -1 : status;
}

int run_user_code(Sandbox *ptr) {
    int fd[2];
    // TODO(conankun): create the random cgroup name.
    auto cgroup = setup_cgroup(ptr, "test_sandbox/subgroup", true);
    if (cgroup == nullptr) {
        return -1;
    }
    if (pipe(fd) == -1) {
        return -1;
    }
    auto pid = fork();
    switch (pid) {
        case -1:
            return -1;
        case 0: /* Child */
            cgroup->attach_process(getpid());
            run_child(ptr, fd);
            exit(EXIT_SUCCESS);
        default: /* Parent */
            return run_parent(ptr, std::move(cgroup), pid, fd);
            break;
    }
    return 0;
}