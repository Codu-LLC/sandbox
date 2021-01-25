//
// Created by Eugene Junghyun Kim on 1/21/2021.
//

#include "file.h"
#include <cstdio>
#include <string>

bool File::write_file(const std::string &file_path, const std::string &str) {
    FILE *fd = fopen64(file_path.c_str(), "w");
    if (fd == NULL) {
        return false;
    }
    if (fputs(str.c_str(), fd) == -1) {
        fclose(fd);
        return false;
    }
    fclose(fd);
    return true;
}

// TODO(conankun): add error handling (probably move to c lib for file io?).
std::string File::read_file(const std::string &file_path) {
    std::string ret;
    FILE *fd = fopen64(file_path.c_str(), "r");
    if (fd == NULL) {
        return std::string();
    }
    constexpr int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    while (true) {
        int count = fread(buffer, sizeof(char), BUFFER_SIZE, fd);
        if (count == -1) {
            fclose(fd);
            return std::string();
        } else if (count == 0) {
            break;
        } else {
            ret += buffer;
        }
    }
    fclose(fd);
    return ret;
}
