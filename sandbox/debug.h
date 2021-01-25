//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#ifndef SANDBOX_DEBUG_H
#define SANDBOX_DEBUG_H

#include "sandbox.h"
#include <string>

extern "C" void print_string(bool debug, std::string str);
extern "C" void debug_process(bool debug);
extern "C" void debug_sandbox_stat(Sandbox *ptr);

#endif //SANDBOX_DEBUG_H
