//
// Created by Eugene Junghyun Kim on 1/7/2021.
//
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "file.h"
#include "sandbox.h"
#include "sandbox_builder.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#define NIL_INTEGER -1

#define NIL_STRING std::string()

ABSL_FLAG(bool, is_debug, false, "If set to true, shows userful debug information");
ABSL_FLAG(long long, time_limit, NIL_INTEGER, "The time limit of the executable code in miliseconds");
ABSL_FLAG(long long, memory_limit, NIL_INTEGER, "The memory limit of the executable code in Megabytes");
ABSL_FLAG(long long, file_size_limit, NIL_INTEGER,
          "The limit of file size the executable code can create (especially stdout)");
ABSL_FLAG(std::string, sandbox_dir, NIL_STRING, "The path for sandbox's working directory");
ABSL_FLAG(std::string, src_root_fs_dir, NIL_STRING,
          "The path for sandbox's root file system is stored (e.g /mnt/gccrootfs)");
ABSL_FLAG(std::string, target_root_fs_dir, NIL_STRING,"The path where sandbox's root file system will be hosted");
ABSL_FLAG(std::string, command, NIL_STRING, "The command for executing the code submitted by the user");
ABSL_FLAG(std::string, statistics_file_path, NIL_STRING, "The path where the statistics from execution will be stored");
ABSL_FLAG(std::string, output_file_path, NIL_STRING, "The path where the output from the user code will be stored");

//
// Before running, don't forget export BAZEL_CXXOPTS="-std=c++17"
// Run with bazel build -c opt //sandbox:app && unshare -m -r ./codu
//

/**
 * The function that checks whether all required flags are specified.
 * Args:
 *     missing_flags: The string vector that will contain information on missing flags.
 *     This will contain names of missing flags.
 *     Should be empty when passed in.
 * Returns:
 *     A boolean value indicating whether all required flags are specified. 
 */
bool has_required_flags(std::vector<std::string> &missing_flags) {
    assert(missing_flags.empty());
    if (absl::GetFlag(FLAGS_time_limit) == NIL_INTEGER) {
        missing_flags.emplace_back("time_limit");
    }
    if (absl::GetFlag(FLAGS_memory_limit) == NIL_INTEGER) {
        missing_flags.emplace_back("memory_limit");
    }
    if (absl::GetFlag(FLAGS_file_size_limit) == NIL_INTEGER) {
        missing_flags.emplace_back("file_size_limit");
    }
    if (absl::GetFlag(FLAGS_sandbox_dir) == NIL_STRING) {
        missing_flags.emplace_back("sandbox_dir");
    }
    if (absl::GetFlag(FLAGS_src_root_fs_dir) == NIL_STRING) {
        missing_flags.emplace_back("src_root_fs_dir");
    }
    if (absl::GetFlag(FLAGS_target_root_fs_dir) == NIL_STRING) {
        missing_flags.emplace_back("target_root_fs_dir");
    }
    if (absl::GetFlag(FLAGS_command) == NIL_STRING) {
        missing_flags.emplace_back("command");
    }
    if (absl::GetFlag(FLAGS_statistics_file_path) == NIL_STRING) {
        missing_flags.emplace_back("statistics_file_path");
    }
    if (absl::GetFlag(FLAGS_output_file_path) == NIL_STRING) {
        missing_flags.emplace_back("output_file_path");
    }
    return missing_flags.empty();
}

/**
 * The function that gets JSON-formatted string for statistics such as time elapsed, memory used, etc.
 * Args:
 *   sandbox: The reference to the sandbox object.
 * Returns:
 *   A string in JSON that contains statistics for the execution of user submitted code.
 */
std::string get_json_statistics(const Sandbox &sandbox) {
    std::string ret;
    ret.append("{");
    ret.append("{\"time_elapsed\": " + std::to_string(sandbox.get_time_elapsed()) + "},");
    ret.append("{\"memory_used\": " + std::to_string(sandbox.get_memory_used()) + "},");
    ret.append("{\"return_code\": " + std::to_string(sandbox.get_return_code()) + "}");
    ret.append("}");
    return ret;
}

int main(int argc, char *argv[]) {
    absl::ParseCommandLine(argc, argv);
    std::vector<std::string> missing_flags;
    if (!has_required_flags(missing_flags)) {
        for (auto missing_flag : missing_flags) {
	    std::cerr << "The required flag \033[1;31m" << missing_flag << "\033[0m is missing." << std::endl;;
	}
	return -1;
    }   

    auto builder = Sandbox::builder();

    auto sandbox = builder
            .set_debug(absl::GetFlag(FLAGS_is_debug))
            .set_time_limit(absl::GetFlag(FLAGS_time_limit))
            .set_memory_limit(absl::GetFlag(FLAGS_memory_limit))
            .set_file_size_limit_in_mb(absl::GetFlag(FLAGS_file_size_limit))
            .set_sandbox_dir(absl::GetFlag(FLAGS_sandbox_dir))
            .set_src_root_fs_dir(absl::GetFlag(FLAGS_src_root_fs_dir))
            .set_target_root_fs_dir(absl::GetFlag(FLAGS_target_root_fs_dir))
            .set_command(absl::GetFlag(FLAGS_command))
            .build();
    sandbox.run();
    File::write_file(absl::GetFlag(FLAGS_statistics_file_path), get_json_statistics(sandbox));
    File::write_file(absl::GetFlag(FLAGS_output_file_path), sandbox.get_output());
}
