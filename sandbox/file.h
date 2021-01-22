//
// Created by Eugene Junghyun Kim on 1/21/2021.
//

#ifndef SANDBOX_FILE_H
#define SANDBOX_FILE_H

#include <string>

class File {
public:
    static int write_file(std::string file_path, std::string str);
    static std::string read_file(std::string file_path);
};

#endif //SANDBOX_FILE_H
