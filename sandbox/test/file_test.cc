//
// Created by Eugene Junghyun Kim on 1/21/2021.
//

#include "sandbox/file.h"
#include "gtest/gtest.h"
#include <cstdio>
#include <string>

static int remove_file(std::string file_path) {
    if (std::remove(file_path.c_str()) != 0) {
        perror("Error while deleting the file");
    }
    return 0;
}

TEST(FileTest, WritingToExistingFileSucceed) {
    // Create the new file named WritingToExistingFileSucceed.txt
    FILE *f = fopen("WritingToExistingFileSucceed.txt", "w");
    fclose(f);
    EXPECT_EQ(File::write_file("WritingToExistingFileSucceed.txt", "test"), true);
    // Remove file.
    remove_file("WritingToExistingFileSucceed.txt");
}

TEST(FileTest, WritingToNonExistingFileSucceed) {
    EXPECT_EQ(File::write_file("WritingToNonExistingFileSucceed.txt", "test"), true);
    // Remove file.
    remove_file("WritingToNonExistingFileSucceed.txt");
}

TEST(FileTest, ReadingExistingFileSucceed) {
    EXPECT_EQ(File::write_file("ReadingExistingFileSucceed.txt", "16348123123"), true);
    EXPECT_EQ(File::read_file("ReadingExistingFileSucceed.txt"), "16348123123");
    // Remove file.
    remove_file("ReadingExistingFileSucceed.txt");
}
