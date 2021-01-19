//
// Created by Eugene Junghyun Kim on 1/15/2021.
//

#include "cgroup.h"
#include "sandbox.h"
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MB_TO_BYTES(x) ((x) << 20)

static int run_command(const std::string &command) {
    auto pid = fork();
    switch (pid) {
        case -1:
            return -1;
            break;
        case 0: /* Child */
        {
            // TODO(conankun): Replace with execve
            if (system(command.c_str()) != 0) {
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
        default: /* Parent */
            int status;
            waitpid(pid, &status, 0);
            return status;
    }
}

bool Cgroup::setup_cgroup() {
    // TODO(conankun): replace with std::format once upgraded to C++20.
    std::string command = "/usr/bin/cgcreate -g " + subsystems + ":/" + cgroup_name;
    return run_command(command) == 0 ? true : false;
}

bool Cgroup::delete_cgroup() {
    // TODO(conankun): replace with std::format once upgraded to C++20.
    std::string command = "/usr/bin/cgdelete -g " + subsystems + ":/" + cgroup_name;
    return run_command(command) == 0 ? true : false;
}

bool Cgroup::set_property(const std::string &controller, const std::string &property, const std::string &value) {
    // TODO(conankun): replace with std::format once upgraded to C++20.
    std::string command = "/usr/bin/cgset -r " + controller + "." + property + "=" + value + " " + cgroup_name;
    return run_command(command) == 0 ? true : false;
}

std::string Cgroup::get_property(const std::string &controller, const std::string &property) {
    std::string filename = "/sys/fs/cgroup/" + controller + "/" + cgroup_name + "/" + controller + "." + property;
    std::ifstream fd(filename);
    std::string ret = "";
    while (fd.good()) {
        ret += fd.get();
    }
    fd.close();
    return ret;
}

bool Cgroup::attach_process(pid_t pid) {
    // TODO(conankun): replace with std::format once upgraded to C++20.
    std::string command =
            "/usr/bin/cgclassify -g " + subsystems + ":/" + cgroup_name + " " + std::to_string(static_cast<uint32_t>(pid));
    return run_command(command) == 0 ? true : false;
}

void Cgroup::add_controller(const std::string &controller) {
    if (!subsystems.empty()) {
        subsystems.append(",");
    }
    subsystems.append(controller);
}