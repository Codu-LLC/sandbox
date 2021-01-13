//
// Created by Eugene Junghyun Kim on 1/7/2021.
//

#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

#include <string>

// Change the root of the process to new_root and put old root in put_old.
int pivot_root(const std::string &new_root, const std::string &put_old);

#endif //SERVER_UTIL_H
