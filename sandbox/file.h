//
// Created by Eugene Junghyun Kim on 1/21/2021.
//

#ifndef SANDBOX_FILE_H
#define SANDBOX_FILE_H

#include <string>

class File {
public:
    static bool write_file(const std::string &file_path, const std::string &str);
    static std::string read_file(const std::string &file_path);
    static int read_pipe(int fd, std::string &content, long long limit_in_bytes);
};

#endif //SANDBOX_FILE_H
