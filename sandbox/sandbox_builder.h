//
// Created by Eugene Junghyun Kim on 1/8/2021.
//

#ifndef SERVER_SANDBOX_BUILDER_H
#define SERVER_SANDBOX_BUILDER_H

#include "sandbox.h"
#include <string>

class SandboxBuilder {
private:
    Sandbox sandbox;
public:
    SandboxBuilder& set_debug(bool debug);

    SandboxBuilder& set_src_root_fs_dir(std::string &&dir);

    SandboxBuilder& set_target_root_fs_dir(std::string &&dir);

    SandboxBuilder& set_sandbox_dir(std::string &&dir);

    SandboxBuilder& set_command(std::string &&command);

    SandboxBuilder& set_time_limit(int time_limit_in_ms);

    SandboxBuilder& set_memory_limit(int memory_limit_in_mb);

    SandboxBuilder& set_file_size_limit_in_mb(int file_size_in_mb);

    Sandbox& build();
};

#endif //SERVER_SANDBOX_BUILDER_H
