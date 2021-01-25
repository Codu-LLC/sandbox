//
// Created by Eugene Junghyun Kim on 1/21/2021.
//

#include "file.h"
#include <cstdio>
#include <string>
#include <unistd.h>
#include <iostream>

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
            ret.append(buffer, count);
        }
    }
    fclose(fd);
    return ret;
}

// The method for reading from the pipe.
// Since pipe is stored in memory, its size cannot be limited by setrlimit(RLIMIT_FSIZE, ...).
// Therefore, the limit on its total size is enforced here.
// Returns
/**
 * The method for reading from the pipe.
 * Since pipe is stored in memory, its size cannot be limited by setrlimit(RLIMIT_FSIZE, ...).
 * Therefore, the limit on its total size is enforced here.
 * @param fd the integer for file descriptor
 * @param content the reference of the string to append bytes from the pipe
 * @param limit_in_bytes the limit on total size of input from the pipe.
 * @return -1 if system error occurs. 0 if terminates without any issue. 1 if total size of the content is greater than
 * the limit.
 */
int File::read_pipe(int fd, std::string &content, long long limit_in_bytes) {
    constexpr int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    for(;;) {
        auto num_read = read(fd, buffer, BUFFER_SIZE);
        if (num_read == -1) {
            return -1;
        } else if (num_read == 0) {
            break;
        } else {
            content.append(buffer, num_read);
            if (content.size() > limit_in_bytes) {
                break;
            }
        }
    }
    return (content.size() > limit_in_bytes) ? 1 : 0;
}
