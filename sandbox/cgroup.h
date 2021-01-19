//
// Created by Eugene Junghyun Kim on 1/15/2021.
//

#ifndef SANDBOX_CGROUP_H
#define SANDBOX_CGROUP_H
#include "sandbox.h"
#include <string>
#include <sys/types.h>

// TODO(conankun): Make cgroup builder.
class Cgroup {
public:
    Cgroup(const std::string &cgroup_name) : cgroup_name(cgroup_name) {}
    bool setup_cgroup();
    bool delete_cgroup();
    bool set_property(const std::string &controller, const std::string &property, const std::string &value);
    std::string get_property(const std::string &controller, const std::string &property);
    bool attach_process(pid_t pid);
    void add_controller(const std::string &controller);
private:
    std::string cgroup_name, subsystems;
};

#endif //SANDBOX_CGROUP_H
