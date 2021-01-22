//
// Created by Eugene Junghyun Kim on 1/21/2021.
//

#include "file.h"
#include <fstream>
#include <string>
#include <iostream>

bool File::write_file(const std::string &file_path, const std::string &str) {
    std::ofstream fd(file_path);
    if (!fd.is_open()) {
        return -1;
    }
    fd << str;
    fd.close();
    return fd.good();
}

// TODO(conankun): add error handling (probably move to c lib for file io?).
std::string File::read_file(const std::string &file_path) {
    std::ifstream fd(file_path);
    if (!fd.is_open()) {
        return std::string();
    }
    std::string ret, str;
    while (getline(fd, str)) {
        ret += str;
    }
    fd.close();
    return ret;
}
