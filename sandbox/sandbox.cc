#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// The stack size is 256MB.
#define STACK_SIZE 268435456

#define MB_TO_BYTES(x) ((x) << 20)
#define MILLISECONDS_TO_SECONDS(x) ((x)/1000.)
#define BUFFER_SIZE 4096

#include "sandbox.h"
#include "sandbox_builder.h"
#include "util.h"
#include <cmath>
#include <errno.h>
#include <iostream>
#include <linux/seccomp.h>
#include <sched.h>
#include <signal.h>
#include <string>
#include <string.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

static char child_stack[STACK_SIZE];

void Sandbox::set_debug(bool debug) {
    this->debug = debug;
}

void Sandbox::set_src_root_fs_dir(std::string &dir) {
    this->src_root_fs_dir = dir;
}

void Sandbox::set_target_root_fs_dir(std::string &dir) {
    this->target_root_fs_dir = dir;
}

void Sandbox::set_sandbox_dir(std::string &dir) {
    this->sandbox_dir = dir;
}

void Sandbox::set_command(std::string &command) {
    this->command = command;
}

void Sandbox::set_time_limit(int time_limit_in_ms) {
    this->time_limit_in_ms = time_limit_in_ms;
}

void Sandbox::set_memory_limit(int memory_limit_in_mb) {
    this->memory_limit_in_mb = memory_limit_in_mb;
}

void Sandbox::set_file_size_limit_in_mb(int file_size_in_mb) {
    this->file_size_limit_in_mb = file_size_in_mb;
}

bool Sandbox::is_debug() const {
    return debug;
}
std::string& Sandbox::get_src_root_fs_dir() {
    return src_root_fs_dir;
}
std::string& Sandbox::get_target_root_fs_dir() {
    return target_root_fs_dir;
}
std::string& Sandbox::get_sandbox_dir() {
    return sandbox_dir;
}
std::string& Sandbox::get_command() {
    return command;
}
int Sandbox::get_time_limit_in_ms() const {
    return time_limit_in_ms;
}
int Sandbox::get_memory_limit_in_mb() const {
    return memory_limit_in_mb;
}

int Sandbox::get_file_size_limit_in_mb() const {
    return file_size_limit_in_mb;
}

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
    // Limit
    // Limit in Wall time. If process is sleeping, they are not consuming cpu time and thus can block grading system
    // from processing future requests. Therefore, there should be an enforcement on wall time
    alarm(static_cast<int>(ceil(MILLISECONDS_TO_SECONDS(ptr->get_time_limit_in_ms()))) << 1);
    set_resource_limit(RLIMIT_CPU, static_cast<int>(ceil(MILLISECONDS_TO_SECONDS(ptr->get_time_limit_in_ms()))));
    // Prevent the process from making system calls other than read, write, sigreturn, and exit.
    // prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);
}

void run_user_code(Sandbox* ptr, int *fd) {
    if (close(fd[0]) == -1) {
        std::cerr << "Failed to close read pipe" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (unshare(CLONE_NEWUSER) == -1) {
        std::cerr << "Unsharing with new user namespace failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (ptr->is_debug()) {
        std::cerr << "PID after unshare: " << getpid() << std::endl;
        std::cerr << "PPID after unshare: " << getppid() << std::endl;
        std::cerr << "Real UID after unshare: " << getuid() << std::endl;
        std::cerr << "Effective UID after unshare: " << geteuid() << std::endl;
    }
    dup2(fd[1], STDOUT_FILENO);
    char *argv[] = {NULL};
    char *env_variable[] = {NULL};
    set_sandbox_limit(ptr);
    execve(ptr->get_command().c_str(), argv, env_variable);
    if (close(fd[1]) == -1) {
        std::cerr << "Failed to close write pipe" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

static int launch_sandbox(void *args) {
    Sandbox* ptr = (Sandbox *) args;
    if (ptr->is_debug()) {
        std::cerr << "PID: " << getpid() << std::endl;
        std::cerr << "PPID: " << getppid() << std::endl;
        std::cerr << "Real UID: " << getuid() << std::endl;
        std::cerr << "Effective UID: " << geteuid() << std::endl;
    }
    if (mount(ptr->get_src_root_fs_dir().c_str(), ptr->get_target_root_fs_dir().c_str(), "", MS_BIND, NULL) == -1) {
        std::cerr << "Binding rootfs not successful: " << strerror(errno) << std::endl;
        return 1;
    }
    // TODO(conankun): Find out an api function for joining paths safely.
    if (mount(ptr->get_sandbox_dir().c_str(),
              (ptr->get_target_root_fs_dir() + "/" + ptr->get_sandbox_dir()).c_str(),
              "",
              MS_BIND,
              NULL) == -1) {
        std::cerr << "Binding sandbox not successful: " << strerror(errno) << std::endl;
        return 1;
    }
    if (chdir(ptr->get_target_root_fs_dir().c_str()) == -1) {
        std::cerr << "Changing directory to rootfs failed: " << strerror(errno) << std::endl;
        return 1;
    }
    if (pivot_root(".", "put_old") == -1) {
        std::cerr << "Pivot root failed: " << strerror(errno) << std::endl;
        return 1;
    }
    if (umount2("put_old", MNT_DETACH) == -1) {
        std::cerr << "Unmounting put_old failed: " << strerror(errno) << std::endl;
        return 1;
    }

    // TODO(conankun): Call these with system call instead of calling system function.
    // This is generating /dev/(u)random so that it can be used to generate random numbers.
    // Also, to run python, having /dev/(u)random are necessary.
    system("/bin/mknod -m 0666 /dev/random c 1 8");
    system("/bin/mknod -m 0666 /dev/urandom c 1 9");
    system("/bin/chown root:root /dev/random /dev/urandom");
    // TODO(conankun): Add cgroup and seccomp.
    int fd[2];
    if (pipe(fd) == -1) {
        std::cerr << "Opening a pipe failed: " << strerror(errno) << std::endl;
        return 1;
    }
    auto pid = fork();
    switch(pid) {
        case -1:
            std::cerr << "Fork failed: " << strerror(errno) << std::endl;
            return 1;
        case 0: /* Child */
            run_user_code(ptr, fd);
            break;
        default: /* Parent */
            if (close(fd[1]) == -1) {
                std::cerr << "Failed to close write pipe" << strerror(errno) << std::endl;
                return 1;
            }
            char buffer[BUFFER_SIZE];
            for(;;) {
                auto num_read = read(fd[0], buffer, BUFFER_SIZE);
                if (num_read == -1) {
                    std::cerr << "Error while reading" << std::endl;
                    break;
                }
                if (num_read == 0) {
                    break;
                }
                write(STDOUT_FILENO, "test", 4);
                write(STDOUT_FILENO, buffer, num_read);
                write(STDOUT_FILENO, "\n", 1);
            }
            if (close(fd[0]) == -1) {
                std::cerr << "Failed to close read pipe" << strerror(errno) << std::endl;
                return 1;
            }
            waitpid(pid, NULL, 0);
    }
    // Run the code provided by the user.
    return 0;
}

SandboxBuilder Sandbox::builder() {
    return SandboxBuilder{};
}

void Sandbox::run() {
    pid_t child_pid = clone(
            launch_sandbox,
            child_stack + STACK_SIZE,
            CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWIPC | CLONE_NEWNET | CLONE_NEWUTS | SIGCHLD,
            (void *) this
    );
    waitpid(child_pid, NULL, 0);
}
