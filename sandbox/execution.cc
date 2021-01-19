//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#include "cgroup.h"
#include "debug.h"
#include "execution.h"
#include "limit.h"
#include <memory>
#include <sched.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>

#define MB_TO_BYTES(x) ((x) << 20)

static void run_child(Sandbox *ptr) {
    if (unshare(CLONE_NEWUSER) == -1) {
        exit(EXIT_FAILURE);
    }
    print_string(ptr->is_debug(), "After unshare");
    debug_process(ptr->is_debug());
    // Pipe stdout to fd[1].
    // dup2(fd[1], STDOUT_FILENO);
    char *argv[] = {NULL};
    char *env_variable[] = {NULL};
    set_sandbox_limit(ptr);
    if (execve(ptr->get_command().c_str(), argv, env_variable) != 0) {
        std::cerr << "execve error" << std::endl;
        perror("error: ");
    }
    /*if (close(fd[1]) == -1) {
        exit(EXIT_FAILURE);
    }*/
}


static int run_parent(Sandbox *ptr, std::unique_ptr<Cgroup> cgroup, pid_t pid) {
    int status;
    waitpid(pid, &status, 0);
    // TODO(conankun): Add a signal catch to diversify verdict.
    system("cat /sys/fs/cgroup/memory/test/memory.max_usage_in_bytes");
    system("cat /sys/fs/cgroup/cpu/test/cpuacct.usage");
    cgroup->delete_cgroup();
}

int run_user_code(Sandbox *ptr, int *fd) {
    if (close(fd[0]) == -1) {
        return -1;
    }
    auto cgroup = setup_cgroup(ptr, "test", true);
    if (cgroup == nullptr) {
        return -1;
    }
    auto pid = fork();
    switch (pid) {
        case -1:
            return -1;
        case 0: /* Child */
            cgroup->attach_process(getpid());
            run_child(ptr);
            exit(EXIT_SUCCESS);
        default: /* Parent */
            return run_parent(ptr, std::move(cgroup), pid);
            break;
    }
    return 0;
}