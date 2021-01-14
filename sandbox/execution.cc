//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#include "debug.h"
#include "execution.h"
#include "limit.h"
#include <sched.h>
#include <unistd.h>

int run_user_code(Sandbox *ptr, int *fd) {
    if (close(fd[0]) == -1) {
        return -1;
    }
    // TODO(conankun): Add cgroup command here. You might need to fork here to cgdelete successfully in parent process.
    if (unshare(CLONE_NEWUSER) == -1) {
        return -1;
    }
    print_string(ptr->is_debug(), "After unshare");
    debug_process(ptr->is_debug());
    // Pipe stdout to fd[1].
    dup2(fd[1], STDOUT_FILENO);
    char *argv[] = {NULL};
    char *env_variable[] = {NULL};
    set_sandbox_limit(ptr);
    execve(ptr->get_command().c_str(), argv, env_variable);
    if (close(fd[1]) == -1) {
        return -1;
    }
    return 0;
}