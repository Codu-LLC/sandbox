//
// Created by Eugene Junghyun Kim on 1/8/2021.
//

#ifndef SANDBOX_SANDBOX_BUILDER_H
#define SANDBOX_SANDBOX_BUILDER_H

#include "sandbox.h"
#include <string>
#include <vector>

class SandboxBuilder {
private:
    Sandbox sandbox;
public:
    SandboxBuilder& set_debug(bool debug);

    SandboxBuilder& set_src_root_fs_dir(std::string &&dir);

    SandboxBuilder& set_target_root_fs_dir(std::string &&dir);

    SandboxBuilder& set_sandbox_dir(std::string &&dir);

    SandboxBuilder& set_command(std::vector<std::string> &&command);

    SandboxBuilder& set_time_limit(int time_limit_in_ms);

    SandboxBuilder& set_memory_limit(int memory_limit_in_mb);

    SandboxBuilder& set_file_size_limit_in_mb(int file_size_in_mb);

    Sandbox& build();
};

#endif //SANDBOX_SANDBOX_BUILDER_H
