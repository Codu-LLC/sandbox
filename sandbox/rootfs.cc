//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#include "debug.h"
#include "rootfs.h"
#include "sandbox.h"
#include "util.h"
#include <fcntl.h>
#include <iostream>
#include <sys/types.h>
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
        print_string(ptr->is_debug(), "Making a directory for sandbox failed");
        return -1;
    }

    if (mount(ptr->get_sandbox_dir().c_str(),
              (ptr->get_target_root_fs_dir() + "/sandbox").c_str(),
              "",
              MS_BIND,
              NULL) == -1) {
        print_string(ptr->is_debug(), "Mounting the sandbox directory failed.");
        return -1;
    }

    // Scope into new root file system directory.
    if (chdir(ptr->get_target_root_fs_dir().c_str()) == -1) {
        print_string(ptr->is_debug(), "Changing directory to the target rootfs failed.");
        return -1;
    }

    if (mkdir("sandbox/put_old", 0777) == -1 && errno != EEXIST) {
        print_string(ptr->is_debug(), "Making a directory for put_old failed.");
        return -1;
    }

    // Change the root to this directory.
    if (pivot_root(".", "sandbox/put_old") == -1) {
        print_string(ptr->is_debug(), "Pivot root failed.");
        return -1;
    }

    // Mount dev, proc, sys, and tmp from old root filesystem.
    // Mount dev.
    if (mount("sandbox/put_old/dev", "dev", "", MS_BIND | MS_REC, NULL) == -1) {
        print_string(ptr->is_debug(), "Mounting /dev failed.");
        return -1;
    }
    if (mount("", "dev", "", MS_SLAVE | MS_REC, NULL) == -1) {
        print_string(ptr->is_debug(), "Mounting /dev failed.");
        return -1;
    }

    // Mount /proc.
    if (mount("sandbox/put_old/proc", "proc", "", MS_BIND | MS_REC, NULL) == -1) {
        print_string(ptr->is_debug(), "Mounting /proc failed.");
        return -1;
    }
    if (mount("", "proc", "", MS_SLAVE | MS_REC, NULL) == -1) {
        print_string(ptr->is_debug(), "Mounting /proc failed.");
        return -1;
    }

    // Mount /sys.
    if (mount("sandbox/put_old/sys", "sys", "", MS_BIND | MS_REC, NULL) == -1) {
        print_string(ptr->is_debug(), "Mounting /sys failed.");
        return -1;
    }
    if (mount("", "sys", "", MS_SLAVE | MS_REC, NULL) == -1) {
        print_string(ptr->is_debug(), "Mounting /sys failed.");
        return -1;
    }

    // Mount /tmp.
    // TODO(conankun): Investigate if this is necessary.
    if (mount("sandbox/put_old/tmp", "tmp", "", MS_BIND | MS_REC, NULL) == -1) {
        print_string(ptr->is_debug(), "Mounting /tmp failed.");
        return -1;
    }

    if (umount2("sandbox/put_old", MNT_DETACH) == -1) {
        print_string(ptr->is_debug(), "Detaching old rootfs failed.");
        return -1;
    }

    if (remove("sandbox/put_old") == -1) {
        print_string(ptr->is_debug(), "Removing old rootfs failed.");
        return -1;
    }

    if (chdir("sandbox") == -1) {
        print_string(ptr->is_debug(), "Changing current directory to sandbox failed.");
        return -1;
    }
    int f = open("stat.txt", O_CREAT, 0777);
    if (f == -1) {
        print_string(ptr->is_debug(), "Creating stat file failed.");
	return -1;
    }
    close(f);

    return 0;
}
