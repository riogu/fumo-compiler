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
    TestConfig() : description("default") {
        compiler_options = {"-print-file", "-print-ast", "-print-ir", "-print-file", "-emit-ir"};
    }
    TestConfig(std::vector<std::string> opts, std::string desc = "") 
        : compiler_options(std::move(opts)), description(std::move(desc)) {}
    
    std::string get_options_string() const {
        std::string result;
        for (const auto& opt : compiler_options) {
            result += opt + " ";
        }
        return result;
    }
};

// Enhanced expected result detection that checks the entire path
ExpectedResult get_expected_result(const std::filesystem::path& file_path) {
    std::string filename = file_path.filename().string();
    std::string full_path = file_path.string();
    
    // Check for explicit markers in filename
    if (filename.find("_fail") != std::string::npos || 
        filename.find("-fail") != std::string::npos ||
        filename.find("fail_") != std::string::npos ||
        filename.find("error_") != std::string::npos) {
        return ExpectedResult::Fail;
    }
    
    // Check for fail/error markers anywhere in the path
    if (full_path.find("/fail") != std::string::npos ||
        full_path.find("\\fail") != std::string::npos ||
        full_path.find("/error") != std::string::npos ||
        full_path.find("\\error") != std::string::npos ||
        full_path.find("-fail") != std::string::npos ||
        full_path.find("_fail") != std::string::npos) {
        return ExpectedResult::Fail;
    }
    
    // Default to pass
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
            
            // Debug: show what files we're finding
            std::string relative_path = std::filesystem::relative(test_file.path, test_dir).string();
            std::print("  Found: {} (expect: {})\n", 
                      relative_path,
                      test_file.expected == ExpectedResult::Pass ? "PASS" : "FAIL");
        }
    }
    
    // Sort for consistent ordering
    std::sort(test_files.begin(), test_files.end(), 
              [](const TestFile& a, const TestFile& b) {
                  return a.path < b.path;
              });
    
    return test_files;
}

void clear_output_directories() {
    std::print("Clearing all output directories...\n");
    int cleared_count = 0;
    
    // Look for output directories in the test directories
    std::string base_test_path = "src/tests/";
    
    if (!std::filesystem::exists(base_test_path)) {
        std::print("Test directory '{}' does not exist!\n", base_test_path);
        return;
    }
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(base_test_path)) {
            if (entry.is_directory()) {
                std::string dir_name = entry.path().filename().string();
                // Check if directory name ends with "outputs"
                if (dir_name.length() >= 7 && 
                    dir_name.substr(dir_name.length() - 7) == "outputs") {
                    std::string relative_path = std::filesystem::relative(entry.path()).string();
                    std::print("  Removing: {}\n", relative_path);
                    std::filesystem::remove_all(entry.path());
                    cleared_count++;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::print("Error clearing directories: {}\n", e.what());
        return;
    }
    
    std::print("Cleared {} output directories.\n", cleared_count);
}

void run_tests(const std::string& test_subdir, const TestConfig& config = TestConfig()) {
    std::string full_path = std::format("src/tests/{}", test_subdir);
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
        std::string relative_path = std::filesystem::relative(test_file.path).string();
        
        // Show subdirectory headers for better organization
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
        
        // Generate output file path
        std::string input_filename = test_file.path.stem().string(); // filename without extension
        std::string output_dir = std::format("{}outputs", input_filename);
        std::string output_filename = std::format("{}/{}", output_dir, input_filename);
        
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(output_dir);
        
        // Build the command with compiler options and output file
        std::string cmd = std::format("ASAN_OPTIONS=detect_leaks=0 ./build/fumo {} -i '{}' -o '{}' 2>&1", 
                                     config.get_options_string(),
                                     test_file.path.string(),
                                     output_filename);
        
        auto [output, status] = exec(cmd.c_str());
        
        bool compiler_succeeded = (WEXITSTATUS(status) == 0);
        bool test_passed = (test_file.expected == ExpectedResult::Pass) ? compiler_succeeded : !compiler_succeeded;
        
        std::string test_name = test_file.path.filename().string();
        
        if (test_passed) {
            std::print("-> \033[38;2;88;154;143m✓ OK\033[0m: {}\n", test_name);
            // Show output for passing tests if verbose mode is enabled
            if (config.verbose_output && !output.empty() && output != "\n") {
                std::print("   Output: \n{}\n", output);
            }
            passed++;
        } else {
            std::print("-> \033[38;2;235;67;54m❌FAILED\033[0m: {}\n", test_name);
            std::print("   Expected: {}, Got: {}\n", 
                      (test_file.expected == ExpectedResult::Pass) ? "PASS" : "FAIL",
                      compiler_succeeded ? "PASS" : "FAIL");
            if (!output.empty()) {
                std::print("   Output: \n{}\n", output);
                std::print("   Command: {}\n", cmd);
            }
            failed++;
        }
    }
    
    std::print("================================================\n");
    std::print("Results: {} passed, {} failed\n", passed, failed);
}

// Predefined test configurations
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
    std::print("  ./test_runner [options] [test_directories...]\n\n");
    std::print("Options:\n");
    std::print("  --config <name>     - Use predefined configuration\n");
    std::print("  --opt <option>      - Add compiler option (can be used multiple times)\n");
    std::print("  --verbose, -v       - Show compiler output for passing tests\n");
    std::print("  --list-configs      - List available configurations\n");
    std::print("  --help              - Show this help\n\n");
    std::print("Examples:\n");
    std::print("  ./test_runner                                    # Run default tests\n");
    std::print("  ./test_runner --verbose                         # Run with verbose output\n");
    std::print("  ./test_runner --config O3 -v early-ast-tests    # Run with O3 optimizations and verbose\n");
    std::print("  ./test_runner --opt --print-ast -v              # Run with AST printing and verbose\n");
    std::print("\nTest Organization:\n");
    std::print("  Tests are automatically discovered recursively in subdirectories.\n");
    std::print("  Files/directories with 'fail' or 'error' in the name are expected to fail.\n");
    std::print("  Example structure:\n");
    std::print("    src/tests/if-statements/\n");
    std::print("    ├── basic/\n");
    std::print("    │   ├── simple_if.fm          (expected: PASS)\n");
    std::print("    │   └── nested_if.fm          (expected: PASS)\n");
    std::print("    └── fail/\n");
    std::print("        ├── missing_condition.fm  (expected: FAIL)\n");
    std::print("        └── invalid_syntax.fm     (expected: FAIL)\n");
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
    return TestConfig(); // Return default if not found
}

// Update main function to handle clear-outputs flag
int main(int argc, char* argv[]) {
    std::vector<std::string> test_dirs;
    TestConfig config;
    bool parsing_test_dirs = false;
    
    // Parse command line arguments
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
        } else if (arg == "--config" && i + 1 < argc) {
            config = find_config(argv[++i]);
        } else if (arg == "--opt" && i + 1 < argc) {
            config.compiler_options.push_back(argv[++i]);
            config.description = "custom";
        } else if (arg == "--verbose" || arg == "-v") {
            config.verbose_output = true;
        } else if (arg == "--") {
            parsing_test_dirs = true;
        } else if (parsing_test_dirs || (!arg.starts_with("-") && !arg.starts_with("--"))) {
            test_dirs.push_back(arg);
        } else {
            // This is a compiler option
            config.compiler_options.push_back(arg);
            config.description = "custom";
        }
    }
    
    // Default test directories if none specified
    if (test_dirs.empty()) {
        test_dirs = {
            // "early-ast-tests",
            "structs-and-postfix", 
            "string-literals",
            // Add more as needed
        };
    }
    
    // Run tests with specified configuration
    for (const auto& test_dir : test_dirs) {
        run_tests(test_dir, config);
        std::print("\n");
    }
    
    return 0;
}
