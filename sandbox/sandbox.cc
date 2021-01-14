#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// The stack size is 256MB.
#define STACK_SIZE 268435456

#include "debug.h"
#include "execution.h"
#include "rootfs.h"
#include "sandbox.h"
#include "sandbox_builder.h"
#include "util.h"
#include <errno.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

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

static int launch_sandbox(void *args) {
    Sandbox *ptr = (Sandbox *) args;
    debug_process(ptr->is_debug());

    if (setup_rootfs(ptr) == -1) {
        std::cerr << "setup rootfs failed" << std::endl;
        return -1;
    }

    // TODO(conankun): Add cgroup and seccomp.
    int fd[2];
    if (pipe(fd) == -1) {
        std::cerr << "Opening a pipe failed: " << strerror(errno) << std::endl;
        return -1;
    }
    // Run the code provided by the user.
    if (run_user_code(ptr, fd) == -1) {
        return -1;
    }
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
