//
// Created by Eugene Junghyun Kim on 1/8/2021.
//

#include "sandbox_builder.h"

#include <string>
#include <vector>

SandboxBuilder& SandboxBuilder::set_debug(bool debug) {
    this->sandbox.set_debug(debug);
    return *this;
}
SandboxBuilder& SandboxBuilder::set_src_root_fs_dir(std::string &&dir) {
    this->sandbox.set_src_root_fs_dir(dir);
    return *this;
}
SandboxBuilder& SandboxBuilder::set_target_root_fs_dir(std::string &&dir) {
    this->sandbox.set_target_root_fs_dir(dir);
    return *this;
}
SandboxBuilder& SandboxBuilder::set_sandbox_dir(std::string &&dir) {
    this->sandbox.set_sandbox_dir(dir);
    return *this;
}
SandboxBuilder& SandboxBuilder::set_command(std::vector<std::string> &&command) {
    this->sandbox.set_command(command);
    return *this;
}
SandboxBuilder& SandboxBuilder::set_time_limit(int time_limit_in_ms) {
    this->sandbox.set_time_limit(time_limit_in_ms);
    return *this;
}
SandboxBuilder& SandboxBuilder::set_memory_limit(int memory_limit_in_mb) {
    this->sandbox.set_memory_limit(memory_limit_in_mb);
    return *this;
}
SandboxBuilder& SandboxBuilder::set_file_size_limit_in_mb(int file_size_in_mb) {
    this->sandbox.set_file_size_limit_in_mb(file_size_in_mb);
    return *this;
}

Sandbox& SandboxBuilder::build() {
    return this->sandbox;
}
