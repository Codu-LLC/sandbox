//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#include "debug.h"
#include "sandbox.h"
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
void debug_sandbox_stat(Sandbox *ptr) {
    if (ptr->is_debug()) {
        std::cerr << "========================= Statistics =========================" << std::endl;
        std::cerr << "Return code: " << ptr->get_return_code() << std::endl;
        std::cerr << "Signal: " << WTERMSIG(ptr->get_return_code()) << std::endl;
        std::cerr << "Time Elapsed: " << ptr->get_time_elapsed() << " ms" << std::endl;
        std::cerr << "Memory Used: " << ptr->get_memory_used() << " MB" << std::endl;
    }
}
