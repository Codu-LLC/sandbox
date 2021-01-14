//
// Created by Eugene Junghyun Kim on 1/13/2021.
//

#include "pipe.h"
#include <string>
#define BUFFER_SIZE 4096

/**
 * Read from file descriptor until it closes and append to str.
 * @param fd integer for file descriptor.
 * @param str string to append characters read from the fd.
 * @return return 0 if successful. return -1 if not successful.
 */
int read(int fd, std::string &str) {
    char buffer[BUFFER_SIZE];
    for (;;) {
        auto num_read = read(fd, buffer, BUFFER_SIZE);
        if (num_read == -1) {
            std::cerr << "Error while reading" << std::endl;
            break;
        }
        if (num_read == 0) {
            break;
        }
        str.append(buffer, num_read);
    }
    if (close(fd) == -1) {
        return -1;
    }
    return 0;
}