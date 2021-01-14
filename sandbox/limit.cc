//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#include "limit.h"
#include "sandbox.h"
#include <cmath>
#include <sys/resource.h>
#include <unistd.h>
#define MB_TO_BYTES(x) ((x) << 20)
#define MILLISECONDS_TO_SECONDS(x) ((x)/1000.)

static void set_resource_limit(int resource, rlim_t limit) {
    struct rlimit rl = {limit, limit};
    if (setrlimit(resource, &rl) == -1) {
        exit(EXIT_FAILURE);
    }
}

void set_sandbox_limit(Sandbox *ptr) {
    set_resource_limit(RLIMIT_AS, MB_TO_BYTES(ptr->get_memory_limit_in_mb()) << 1);
    // Set Maximum file size to FILE_SIZE_LIMIT_IN_MB.
    set_resource_limit(RLIMIT_FSIZE, MB_TO_BYTES(ptr->get_file_size_limit_in_mb()));
    // Limit creating core dump files.
    set_resource_limit(RLIMIT_CORE, 0);
    // Limit sending and receiving message through message queue.
    set_resource_limit(RLIMIT_MSGQUEUE, 0);
    // Set the limit on the number of open file descriptors.
    set_resource_limit(RLIMIT_NOFILE, 10);
    // Limit creating another thread or process.
    set_resource_limit(RLIMIT_NPROC, 0);
    // Limit in Wall time. If process is sleeping, they are not consuming cpu time and thus can block grading system
    // from processing future requests. Therefore, there should be an enforcement on wall time
    alarm(static_cast<int>(ceil(MILLISECONDS_TO_SECONDS(ptr->get_time_limit_in_ms()))) << 1);
    set_resource_limit(RLIMIT_CPU, static_cast<int>(ceil(MILLISECONDS_TO_SECONDS(ptr->get_time_limit_in_ms()))));
    // Prevent the process from making system calls other than read, write, sigreturn, and exit.
    // prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);
}
