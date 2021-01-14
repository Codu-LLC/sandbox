#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// The stack size is 256MB.
#define STACK_SIZE 268435456

#include "limit.h"
#include "rootfs.h"
#include "sandbox.h"
#include "sandbox_builder.h"
#include "util.h"
#include <errno.h>
#include <iostream>
#include <linux/seccomp.h>
#include <sched.h>
#include <signal.h>
#include <string>
#include <string.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/stat.h>
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

std::string &Sandbox::get_src_root_fs_dir() {
    return src_root_fs_dir;
}

std::string &Sandbox::get_target_root_fs_dir() {
    return target_root_fs_dir;
}

std::string &Sandbox::get_sandbox_dir() {
    return sandbox_dir;
}

std::string &Sandbox::get_command() {
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

void run_user_code(Sandbox *ptr, int *fd) {
    if (close(fd[0]) == -1) {
        std::cerr << "Failed to close read pipe" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    // TODO(conankun): Add cgroup command here. You might need to fork here to cgdelete successfully in parent process.
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
    Sandbox *ptr = (Sandbox *) args;
    if (ptr->is_debug()) {
        std::cerr << "PID: " << getpid() << std::endl;
        std::cerr << "PPID: " << getppid() << std::endl;
        std::cerr << "Real UID: " << getuid() << std::endl;
        std::cerr << "Effective UID: " << geteuid() << std::endl;
    }

    if (setup_rootfs(ptr) == -1) {
        std::cerr << "setup rootfs failed" << std::endl;
        return -1;
    }

    // TODO(conankun): Add cgroup and seccomp.
    int fd[2];
    if (pipe(fd) == -1) {
        std::cerr << "Opening a pipe failed: " << strerror(errno) << std::endl;
        return 1;
    }
    // Run the code provided by the user.
    run_user_code(ptr, fd);
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
