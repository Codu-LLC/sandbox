//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#ifndef SANDBOX_EXECUTION_H
#define SANDBOX_EXECUTION_H

#include "sandbox.h"

extern "C" int run_user_code(Sandbox *ptr, int *fd);

#endif //SANDBOX_EXECUTION_H
