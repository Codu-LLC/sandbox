//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#include "rootfs.h"
#include "sandbox.h"
#include "util.h"
#include <iostream>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

int setup_rootfs(Sandbox *ptr) {
    // Mount root file system.
    if (mount(ptr->get_src_root_fs_dir().c_str(), ptr->get_target_root_fs_dir().c_str(), "", MS_BIND | MS_REC, NULL) ==
        -1) {
        return -1;
    }

    // TODO(conankun): Find out an api function for joining paths safely.
    // Mount sandbox directory that contains user submitted code.
    // This will be the place where user submitted code will be compiled and run.
    // Also, randomize sandbox directory name in target directory.
    if (mkdir((ptr->get_target_root_fs_dir() + "/sandbox").c_str(), 0777) == -1 && errno != EEXIST) {
        return -1;
    }

    if (mount(ptr->get_sandbox_dir().c_str(),
              (ptr->get_target_root_fs_dir() + "/sandbox").c_str(),
              "",
              MS_BIND,
              NULL) == -1) {
        return -1;
    }

    // Scope into new root file system directory.
    if (chdir(ptr->get_target_root_fs_dir().c_str()) == -1 && errno != EEXIST) {
        return -1;
    }

    if (mkdir("put_old", 0700) == -1) {
        return -1;
    }

    // Change the root to this directory.
    if (pivot_root(".", "put_old") == -1) {
        return -1;
    }

    // Mount dev, proc, sys, and tmp from old root filesystem.
    // Mount dev.
    if (mount("put_old/dev", "dev", "", MS_BIND | MS_REC, NULL) == -1) {
        return -1;
    }
    if (mount("", "dev", "", MS_SLAVE | MS_REC, NULL) == -1) {
        return -1;
    }

    // Mount /proc.
    if (mount("put_old/proc", "proc", "", MS_BIND | MS_REC, NULL) == -1) {
        return -1;
    }
    if (mount("", "proc", "", MS_SLAVE | MS_REC, NULL) == -1) {
        return -1;
    }

    // Mount /sys.
    if (mount("put_old/sys", "sys", "", MS_BIND | MS_REC, NULL) == -1) {
        return -1;
    }
    if (mount("", "sys", "", MS_SLAVE | MS_REC, NULL) == -1) {
        return -1;
    }

    // Mount /tmp.
    // TODO(conankun): Investigate if this is necessary.
    if (mount("put_old/tmp", "tmp", "", MS_BIND | MS_REC, NULL) == -1) {
        return -1;
    }

    if (umount2("put_old", MNT_DETACH) == -1) {
        return -1;
    }

    if (remove("put_old") == -1) {
        return -1;
    }

    return 0;
}
