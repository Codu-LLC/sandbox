//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#include "debug.h"

#include <iostream>
#include <string>
#include <unistd.h>

void print_string(bool debug, std::string str) {
    if (debug) {
        std::cerr << str << std::endl;
    }
}

void debug_process(bool debug) {
    if (debug) {
        std::cerr << "PID: " << getpid() << std::endl;
        std::cerr << "PPID: " << getppid() << std::endl;
        std::cerr << "Real UID: " << getuid() << std::endl;
        std::cerr << "Effective UID: " << geteuid() << std::endl;
    }
}