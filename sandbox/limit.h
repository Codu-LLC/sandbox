//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#ifndef SANDBOX_LIMIT_H
#define SANDBOX_LIMIT_H

#include "cgroup.h"
#include "sandbox.h"
#include <memory>
#include <string>

extern "C" std::unique_ptr<Cgroup> setup_cgroup(Sandbox *ptr, const std::string &cgroup_name, bool create);
extern "C" void set_sandbox_limit(Sandbox *ptr);

#endif //SANDBOX_LIMIT_H
