//
// Created by Eugene Junghyun Kim on 1/7/2021.
//

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "util.h"

#include <unistd.h>
#include <string>
#include <sys/syscall.h>

int pivot_root(const std::string &new_root, const std::string &put_old) {
    return syscall(SYS_pivot_root, new_root.c_str(), put_old.c_str());
}
