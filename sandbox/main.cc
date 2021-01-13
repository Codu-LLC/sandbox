//
// Created by Eugene Junghyun Kim on 1/7/2021.
//
#include "sandbox.h"
#include "sandbox_builder.h"

// Run with cmake . && make && unshare -m -r ./codu < test.in
int main() {
    auto builder = Sandbox::builder();

    auto sandbox = builder
            .set_debug(true)
            .set_time_limit(1000)
            .set_memory_limit(32)
            .set_sandbox_dir("sandbox")
            .set_src_root_fs_dir("rootfs")
            .set_target_root_fs_dir("rootfs")
            .set_command("./sandbox/test")
            .build();
    sandbox.run();
}
