#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// The stack size is 256MB.
#define STACK_SIZE 268435456
#define PAGE_SIZE 4096

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

void Sandbox::set_command(std::vector<std::string> &command) {
    this->command = std::move(command);
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

std::vector<std::string> &Sandbox::get_command() {
    return command;
}

long long Sandbox::get_time_limit_in_ms() const {
    return time_limit_in_ms;
}

long long Sandbox::get_memory_limit_in_mb() const {
    return memory_limit_in_mb;
}

long long Sandbox::get_file_size_limit_in_mb() const {
    return file_size_limit_in_mb;
}

void Sandbox::set_time_elapsed(long long nanoseconds) {
    time_elapsed_in_ms = nanoseconds / 1000000;
}

void Sandbox::set_memory_used(long long bytes) {
    memory_used_in_mb = bytes >> 20;
}

long long Sandbox::get_time_elapsed() const {
    return time_elapsed_in_ms;
}

long long Sandbox::get_memory_used() const {
    return memory_used_in_mb;
}

void Sandbox::set_return_code(int return_code) {
    this->return_code = return_code;
}
int Sandbox::get_return_code() const {
    return return_code;
}

/**
 * The function that gets JSON-formatted string for statistics such as time elapsed, memory used, etc.
 * Args:
 *   sandbox: The reference to the sandbox object.
 * Returns:
 *   A string in JSON that contains statistics for the execution of user submitted code.
 */
static std::string get_json_statistics(const Sandbox &sandbox) {
    std::string ret;
    ret.append("{");
    ret.append("\"time_elapsed\": " + std::to_string(sandbox.get_time_elapsed()) + ",");
    ret.append("\"memory_used\": " + std::to_string(sandbox.get_memory_used()) + ",");
    ret.append("\"return_code\": " + std::to_string(sandbox.get_return_code()));
    ret.append("}");
    return ret;
}

static int launch_sandbox(void *args) {
    Sandbox *ptr = (Sandbox *) args;
    if (close(ptr->get_fd()[0]) == -1) {
        exit(EXIT_FAILURE);
    }
    debug_process(ptr->is_debug());

    if (setup_rootfs(ptr) == -1) {
        std::cerr << "setup rootfs failed" << std::endl;
        return -1;
    }

    // TODO(conankun): Add seccomp.
    // Run the code provided by the user.
    if (run_user_code(ptr) == -1) {
        return -1;
    }
    auto stat = get_json_statistics(*ptr);
    write(ptr->get_fd()[1], stat.c_str(), stat.size());
    if (close(ptr->get_fd()[1]) == -1) {
        exit(EXIT_FAILURE);
    }
    return 0;
}

SandboxBuilder Sandbox::builder() {
    return SandboxBuilder{};
}

void Sandbox::run() {
    int fd[2];
    if (pipe(fd) == -1) {
        exit(EXIT_FAILURE);
    }
    set_fd(fd);
    if (close(fd[1]) == -1) {
        exit(EXIT_FAILURE);
    }
    pid_t child_pid = clone(
            launch_sandbox,
            child_stack + STACK_SIZE,
            CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWIPC | CLONE_NEWNET | CLONE_NEWUTS | SIGCHLD,
            (void *) this
    );
    waitpid(child_pid, NULL, 0);
    char stat[PAGE_SIZE];
    auto num_read = read(fd[0], stat, PAGE_SIZE);
    if (num_read == -1) {
        exit(EXIT_FAILURE);
    }
    std::cerr << stat << std::endl;
    if (close(fd[0]) == -1) {
        exit(EXIT_FAILURE);
    }
}
