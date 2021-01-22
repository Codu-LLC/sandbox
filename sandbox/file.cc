//
// Created by Eugene Junghyun Kim on 1/21/2021.
//

#include "file.h"
#include <fstream>
#include <string>

int File::write_file(std::string file_path, std::string str) {
    std::ofstream fd(file_path);
    if (!fd.is_open()) {
        return -1;
    }
    fd << str;
    fd.close();
    return 0;
}

std::string File::read_file(std::string file_path) {
    std::ifstream fd(file_path);
    if (!fd.is_open()) {
        return NULL;
    }
    std::string ret, str;
    while (getline(fd, str)) {
        ret += str;
    }
    fd.close();
    return ret;
}
