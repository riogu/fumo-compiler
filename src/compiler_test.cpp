#include <cstdlib>
#include <format>
#include <print>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>

std::pair<std::string, int> exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    int status = pclose(pipe);
    return {result, status};
}

void force_permissions_recursive(const std::filesystem::path& path) {
    std::error_code ec;
    
    // Set permissions on the directory itself
    std::filesystem::permissions(path, 
        std::filesystem::perms::owner_all, 
        std::filesystem::perm_options::replace, ec);
    
    try {
        // Recursively set permissions on all contents
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_directory()) {
                std::filesystem::permissions(entry.path(), 
                    std::filesystem::perms::owner_all, 
                    std::filesystem::perm_options::replace, ec);
            } else if (entry.is_regular_file()) {
                std::filesystem::permissions(entry.path(), 
                    std::filesystem::perms::owner_all, 
                    std::filesystem::perm_options::replace, ec);
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Ignore permission errors during cleanup
    }
}

bool try_force_remove(const std::filesystem::path& path) {
    // Try using system commands as fallback
#ifdef _WIN32
    std::string cmd = std::format("rmdir /s /q \"{}\"", path.string());
#else
    std::string cmd = std::format("chmod -R 755 \"{}\" && rm -rf \"{}\"", 
                                  path.string(), path.string());
#endif
    
    auto [output, status] = exec(cmd.c_str());
    return WEXITSTATUS(status) == 0;
}

bool run_rebuild() {
    std::print("Running rebuild script...\n");
    std::print("================================================\n");
    
    auto [output, status] = exec("bash rebuild.sh 2>&1");
    
    if (!output.empty()) {
        std::print("{}", output);
    }
    
    bool rebuild_succeeded = (WEXITSTATUS(status) == 0);
    
    if (rebuild_succeeded) {
        std::print("================================================\n");
        std::print("\033[38;2;88;154;143m✓ Rebuild successful\033[0m\n");
        std::print("================================================\n");
    } else {
        std::print("================================================\n");
        std::print("\033[38;2;235;67;54m❌ Rebuild failed\033[0m\n");
        std::print("Exit code: {}\n", WEXITSTATUS(status));
        std::print("================================================\n");
    }
    
    return rebuild_succeeded;
}

enum class ExpectedResult {
    Pass,
    Fail
};

struct TestFile {
    std::filesystem::path path;
    ExpectedResult expected;
};

struct TestConfig {
    std::vector<std::string> compiler_options;
    std::string description;
    bool verbose_output = false;
    bool has_custom_options = false;
    
    TestConfig() : description("default") {
        compiler_options = {"-print-file", "-emit-ir-O0", "-emit-ir"};
        has_custom_options = false;
    }
    
    TestConfig(std::vector<std::string> opts, std::string desc = "") 
        : compiler_options(std::move(opts)), description(std::move(desc)), has_custom_options(true) {}
    
    void add_custom_option(const std::string& option) {
        if (!has_custom_options) {
            compiler_options.clear();
            has_custom_options = true;
            description = "custom";
        }
        compiler_options.push_back(option);
    }
    
    std::string get_options_string() const {
        std::string result;
        for (const auto& opt : compiler_options) {
            result += opt + " ";
        }
        return result;
    }
};

ExpectedResult get_expected_result(const std::filesystem::path& file_path) {
    std::string filename = file_path.filename().string();
    std::string full_path = file_path.string();
    
    if (filename.find("_fail") != std::string::npos || 
        filename.find("-fail") != std::string::npos ||
        filename.find("fail_") != std::string::npos ||
        filename.find("error_") != std::string::npos) {
        return ExpectedResult::Fail;
    }
    
    if (full_path.find("/fail") != std::string::npos ||
        full_path.find("\\fail") != std::string::npos ||
        full_path.find("/error") != std::string::npos ||
        full_path.find("\\error") != std::string::npos ||
        full_path.find("-fail") != std::string::npos ||
        full_path.find("_fail") != std::string::npos) {
        return ExpectedResult::Fail;
    }
    
    return ExpectedResult::Pass;
}

std::vector<TestFile> collect_test_files(const std::string& test_dir) {
    std::vector<TestFile> test_files;
    
    if (!std::filesystem::exists(test_dir)) {
        std::print("Test directory '{}' does not exist!\n", test_dir);
        return test_files;
    }
    
    std::print("Scanning directory: {}\n", test_dir);
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(test_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".fm") {
            TestFile test_file;
            test_file.path = entry.path();
            test_file.expected = get_expected_result(entry.path());
            test_files.push_back(test_file);
            
            std::string relative_path = std::filesystem::relative(test_file.path, test_dir).string();
            std::print("  Found: {} (expect: {})\n", 
                      relative_path,
                      test_file.expected == ExpectedResult::Pass ? "PASS" : "FAIL");
        }
    }
    
    std::sort(test_files.begin(), test_files.end(), 
              [](const TestFile& a, const TestFile& b) {
                  return a.path < b.path;
              });
    
    return test_files;
}

TestFile create_single_test_file(const std::string& file_path) {
    TestFile test_file;
    test_file.path = std::filesystem::path(file_path);
    test_file.expected = get_expected_result(test_file.path);
    return test_file;
}

void clear_output_directories() {
    std::print("Clearing all output directories...\n");
    int cleared_count = 0;
    
    std::string base_test_path = "tests/";
    
    if (!std::filesystem::exists(base_test_path)) {
        std::print("Test directory '{}' does not exist!\n", base_test_path);
        return;
    }
    
    std::vector<std::filesystem::path> dirs_to_remove;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(base_test_path)) {
            if (entry.is_directory()) {
                std::string dir_name = entry.path().filename().string();
                if (dir_name.length() >= 8 && 
                    dir_name.substr(dir_name.length() - 8) == "-outputs") {
                    dirs_to_remove.push_back(entry.path());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::print("Error scanning directories: {}\n", e.what());
        return;
    }
    
    for (const auto& dir_path : dirs_to_remove) {
        try {
            std::string relative_path = std::filesystem::relative(dir_path).string();
            std::print("  Removing: {}\n", relative_path);
            
            // First, make sure we have permissions to delete everything
            force_permissions_recursive(dir_path);
            
            std::filesystem::remove_all(dir_path);
            cleared_count++;
        } catch (const std::filesystem::filesystem_error& e) {
            std::print("Error removing directory '{}': {}\n", dir_path.string(), e.what());
            
            // Try alternative removal method
            if (try_force_remove(dir_path)) {
                cleared_count++;
                std::print("  Successfully removed using alternative method\n");
            }
        }
    }
    
    std::print("Cleared {} output directories.\n", cleared_count);
}

void move_compiler_outputs_to_directory(const std::filesystem::path& test_file_path, 
                                       const std::filesystem::path& output_dir) {
    std::filesystem::path test_dir = test_file_path.parent_path();
    std::string base_filename = test_file_path.stem().string();
    
    std::vector<std::string> output_extensions = {
        ".ll", ".asm", ".s", ".o", ".out", ".ast", ""
    };
    
    std::vector<std::string> optimization_suffixes = {
        "-O0", "-O1", "-O2", "-O3"
    };
    
    std::vector<std::filesystem::path> files_to_move;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(test_dir)) {
            if (!entry.is_regular_file()) continue;
            
            std::string filename = entry.path().filename().string();
            std::string extension = entry.path().extension().string();
            
            if (extension == ".fm") continue;
            
            bool should_move = false;
            
            if (filename.starts_with(base_filename)) {
                for (const auto& ext : output_extensions) {
                    if (filename == base_filename + ext) {
                        should_move = true;
                        break;
                    }
                }
                
                if (!should_move) {
                    for (const auto& opt_suffix : optimization_suffixes) {
                        for (const auto& ext : output_extensions) {
                            if (filename == base_filename + opt_suffix + ext) {
                                should_move = true;
                                break;
                            }
                        }
                        if (should_move) break;
                    }
                }
            }
            
            if (should_move) {
                files_to_move.push_back(entry.path());
            }
        }
        
        for (const auto& file_path : files_to_move) {
            std::filesystem::path destination = output_dir / file_path.filename();
            try {
                std::filesystem::create_directories(output_dir);
                std::filesystem::rename(file_path, destination);
            } catch (const std::filesystem::filesystem_error& e) {
                std::print("   Warning: Failed to move {} to {}: {}\n", 
                          file_path.string(), destination.string(), e.what());
            }
        }
        
    } catch (const std::filesystem::filesystem_error& e) {
        std::print("   Warning: Error scanning directory {}: {}\n", 
                  test_dir.string(), e.what());
    }
}

void run_single_test(const std::string& file_path, const TestConfig& config = TestConfig()) {
    if (!std::filesystem::exists(file_path)) {
        std::print("Test file '{}' does not exist!\n", file_path);
        return;
    }
    
    std::filesystem::path test_path(file_path);
    if (test_path.extension() != ".fm") {
        std::print("Warning: File '{}' does not have .fm extension\n", file_path);
    }
    
    TestFile test_file = create_single_test_file(file_path);
    
    std::print("\nRunning single test: {} ({})\n", file_path, config.description);
    if (!config.compiler_options.empty()) {
        std::print("Compiler options: {}\n", config.get_options_string());
    }
    if (config.verbose_output) {
        std::print("Verbose output: enabled\n");
    }
    std::print("Expected result: {}\n", 
              test_file.expected == ExpectedResult::Pass ? "PASS" : "FAIL");
    std::print("================================================\n");
    
    std::string input_filename = test_file.path.stem().string();
    std::filesystem::path test_dir = test_file.path.parent_path();
    std::string output_dir_name = input_filename + "-outputs";
    std::filesystem::path output_dir = test_dir / output_dir_name;
    std::filesystem::path output_file = output_dir / input_filename;

    std::filesystem::create_directories(output_dir);
    std::error_code ec;
    std::filesystem::permissions(output_dir,
                                        std::filesystem::perms::owner_all | std::filesystem::perms::group_read
                                     | std::filesystem::perms::group_exec | std::filesystem::perms::others_read
                                     | std::filesystem::perms::others_exec,
                                 std::filesystem::perm_options::replace,
                                 ec);

    std::string cmd = std::format("./build/fumo '{}' {} -o '{}' 2>&1", 
                                  test_file.path.string(),
                                  config.get_options_string(),
                                  output_file.string());
    
    auto [output, status] = exec(cmd.c_str());
    
    move_compiler_outputs_to_directory(test_file.path, output_dir);
    
    bool compiler_succeeded = (WEXITSTATUS(status) == 0);
    bool test_passed = (test_file.expected == ExpectedResult::Pass) ? compiler_succeeded : !compiler_succeeded;
    
    std::string test_name = test_file.path.filename().string();
    
    if (test_passed) {
        std::print("-> \033[38;2;88;154;143m✓ OK\033[0m: {}\n", test_name);
        std::print("   Output: {}\n", output_file.string());
        if (config.verbose_output && !output.empty() && output != "\n") {
            std::print("   Compiler Output: \n{}\n", output);
        }
    } else {
        std::print("-> \033[38;2;235;67;54m❌FAILED\033[0m: {}\n", test_name);
        std::print("   Expected: {}, Got: {}\n", 
                  (test_file.expected == ExpectedResult::Pass) ? "PASS" : "FAIL",
                  compiler_succeeded ? "PASS" : "FAIL");
        if (!output.empty()) {
            std::print("   Compiler Output: \n{}\n", output);
            std::print("   Command: {}\n", cmd);
        }
    }
    
    std::print("================================================\n");
    std::print("Result: {}\n", test_passed ? "PASSED" : "FAILED");
}

void run_tests(const std::string& test_subdir, const TestConfig& config = TestConfig()) {
    std::string full_path = std::format("tests/{}", test_subdir);
    std::vector<TestFile> test_files = collect_test_files(full_path);
    
    if (test_files.empty()) {
        std::print("No test files found in '{}'\n", full_path);
        return;
    }
    
    std::print("\nRunning tests from: {} ({})\n", full_path, config.description);
    if (!config.compiler_options.empty()) {
        std::print("Compiler options: {}\n", config.get_options_string());
    }
    if (config.verbose_output) {
        std::print("Verbose output: enabled\n");
    }
    std::print("Found {} test files\n", test_files.size());
    std::print("================================================\n");
    
    int passed = 0;
    int failed = 0;
    std::string current_subdir = "";
    
    for (const auto& test_file : test_files) {
        std::string test_subdir_path = std::filesystem::relative(test_file.path.parent_path(), full_path).string();
        if (test_subdir_path != current_subdir) {
            if (!current_subdir.empty()) {
                std::print("------------------------------------------------\n");
            }
            current_subdir = test_subdir_path;
            if (!current_subdir.empty() && current_subdir != ".") {
                std::print("Subdirectory: {}\n", current_subdir);
                std::print("------------------------------------------------\n");
            }
        }
        
        std::string input_filename = test_file.path.stem().string();
        std::filesystem::path test_dir = test_file.path.parent_path();
        std::string output_dir_name = input_filename + "-outputs";
        std::filesystem::path output_dir = test_dir / output_dir_name;
        std::filesystem::path output_file = output_dir / input_filename;

        std::filesystem::create_directories(output_dir);
        std::error_code ec;
        std::filesystem::permissions(output_dir,
                                     std::filesystem::perms::owner_all | std::filesystem::perms::group_read
                                         | std::filesystem::perms::group_exec | std::filesystem::perms::others_read
                                         | std::filesystem::perms::others_exec,
                                     std::filesystem::perm_options::replace,
                                     ec);

        std::string cmd = std::format("./build/fumo '{}' {} -o '{}' 2>&1", 
                                      test_file.path.string(),
                                      config.get_options_string(),
                                      output_file.string());
        
        auto [output, status] = exec(cmd.c_str());
        
        move_compiler_outputs_to_directory(test_file.path, output_dir);
        
        bool compiler_succeeded = (WEXITSTATUS(status) == 0);
        bool test_passed = (test_file.expected == ExpectedResult::Pass) ? compiler_succeeded : !compiler_succeeded;
        
        std::string test_name = test_file.path.filename().string();
        
        if (test_passed) {
            std::print("-> \033[38;2;88;154;143m✓ OK\033[0m: {}\n", test_name);
            std::print("   Output: {}\n", output_file.string());
            if (config.verbose_output && !output.empty() && output != "\n") {
                std::print("   Compiler Output: \n{}\n", output);
            }
            passed++;
        } else {
            std::print("-> \033[38;2;235;67;54m❌FAILED\033[0m: {}\n", test_name);
            std::print("   Expected: {}, Got: {}\n", 
                      (test_file.expected == ExpectedResult::Pass) ? "PASS" : "FAIL",
                      compiler_succeeded ? "PASS" : "FAIL");
            if (!output.empty()) {
                std::print("   Compiler Output: \n{}\n", output);
                std::print("   Command: {}\n", cmd);
            }
            failed++;
        }
    }
    
    std::print("================================================\n");
    std::print("Results: {} passed, {} failed\n", passed, failed);
}

std::vector<TestConfig> get_test_configurations() {
    return {
        TestConfig({"-print-file", "-print-ast", "-print-ir", "-print-file", "-emit-ir"}, "default"),
        TestConfig({"--O0"}, "O0"),
        TestConfig({"--O1"}, "O1"),
        TestConfig({"--O2"}, "O2"),
        TestConfig({"--O3"}, "O3"),
        TestConfig({"--print-ast"}, "print-ast"),
        TestConfig({"--emit-ast"}, "emit-ast"),
        TestConfig({"--emit-ir"}, "emit-ir"),
        TestConfig({"--emit-asm"}, "emit-asm"),
        TestConfig({"-c"}, "compile-only"),
        TestConfig({"--static"}, "static"),
        TestConfig({"-v"}, "verbose"),
    };
}

void print_help() {
    std::print("Test Runner Usage:\n");
    std::print("  ./test_runner [options] [test_directories...]\n");
    std::print("  ./test_runner --test <file_path> [options]\n\n");
    std::print("Options:\n");
    std::print("  --test <file>       - Run a single test file (can be relative or absolute path)\n");
    std::print("  --config <name>     - Use predefined configuration\n");
    std::print("  --opt <option>      - Add compiler option (can be used multiple times)\n");
    std::print("  --verbose, -v       - Show compiler output for passing tests\n");
    std::print("  --clear-outputs     - Clear all *-outputs directories and exit\n");
    std::print("  --list-configs      - List available configurations\n");
    std::print("  --help              - Show this help\n\n");
    std::print("Examples:\n");
    std::print("  ./test_runner                                              # Run default tests\n");
    std::print("  ./test_runner --clear-outputs                              # Clear all output directories\n");
    std::print("  ./test_runner --verbose                                    # Run with verbose output\n");
    std::print("  ./test_runner --config O3 -v early-ast-tests               # Run with O3 optimizations and verbose\n");
    std::print("  ./test_runner --opt --print-ast -v                         # Run with AST printing and verbose\n");
    std::print("  ./test_runner --test tests/if-statements/basic/test.fm # Run single test file\n");
    std::print("  ./test_runner --test ../tests/if-statements/fail/bad.fm -v # Run single test with verbose\n");
    std::print("  ./test_runner --test comparison-operators.fm --config O2   # Run single test with O2 config\n");
    std::print("\nTest Organization:\n");
    std::print("  Tests are automatically discovered recursively in subdirectories.\n");
    std::print("  Files/directories with 'fail' or 'error' in the name are expected to fail.\n");
    std::print("  Output files are created in directories named '<filename>-outputs/'.\n");
    std::print("  Single test files can be specified with absolute or relative paths.\n");
    std::print("  Example structure:\n");
    std::print("    tests/if-statements/\n");
    std::print("    ├── basic/\n");
    std::print("    │   ├── simple_if.fm          (expected: PASS)\n");
    std::print("    │   └── nested_if.fm          (expected: PASS)\n");
    std::print("    └── fail/\n");
    std::print("        ├── missing_condition.fm  (expected: FAIL)\n");
    std::print("        └── invalid_syntax.fm     (expected: FAIL)\n");
    std::print("    Output directories:\n");
    std::print("    ├── simple_if-outputs/\n");
    std::print("    ├── nested_if-outputs/\n");
    std::print("    └── missing_condition-outputs/\n");
}

void list_configurations() {
    std::print("Available configurations:\n");
    for (const auto& config : get_test_configurations()) {
        std::print("  {} - {}\n", config.description, config.get_options_string());
    }
}

TestConfig find_config(const std::string& name) {
    auto configs = get_test_configurations();
    for (const auto& config : configs) {
        if (config.description == name) {
            return config;
        }
    }
    return TestConfig();
}

int main(int argc, char* argv[]) {
    std::vector<std::string> test_dirs;
    std::string single_test_file;
    TestConfig config {};
    bool parsing_test_dirs = false;
    bool skip_rebuild = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h" || arg == "-help") {
            print_help();
            return 0;
        } else if (arg == "--clear-outputs") {
            clear_output_directories();
            return 0;
        } else if (arg == "--list-configs") {
            list_configurations();
            return 0;
        } else if (arg == "--skip-rebuild") {
            skip_rebuild = true;
        } else if (arg == "--test" && i + 1 < argc) {
            single_test_file = argv[++i];
        } else if (arg == "--config" && i + 1 < argc) {
            config = find_config(argv[++i]);
        } else if (arg == "--opt" && i + 1 < argc) {
            config.add_custom_option(argv[++i]);
        } else if (arg == "--verbose" || arg == "-v") {
            config.verbose_output = true;
        } else if (arg == "--") {
            parsing_test_dirs = true;
        } else if (parsing_test_dirs || (!arg.starts_with("-") && !arg.starts_with("--"))) {
            test_dirs.push_back(arg);
        } else {
            config.add_custom_option(arg);
        }
    }
    
    if (!skip_rebuild) {
        if (!run_rebuild()) {
            std::print("Build failed! Exiting without running tests.\n");
            return 1;
        }
        std::print("\n");
    }
    
    if (!single_test_file.empty()) {
        run_single_test(single_test_file, config);
        return 0;
    }
    
    if (test_dirs.empty()) {
        test_dirs = {
            "structs-and-postfix", 
            "string-literals",
            "initializer-lists",
            "fail-init-lists",
            "if-statements",
            "pointer-tests",
            "code-examples",
            "control-flow",
            "fail-control-flow",
            "char-literals",
            "static-member-functions",
            "language-examples",
        };
    }
    
    for (const auto& test_dir : test_dirs) {
        run_tests(test_dir, config);
        std::print("\n");
    }
    
    return 0;
}
