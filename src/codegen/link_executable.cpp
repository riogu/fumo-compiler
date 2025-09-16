#include "codegen/llvm_codegen.hpp"

Result<void, str> link_executable(const LinkOptions& opts) {

    vec<str> cmd;
    LinkerType linker = opts.linker;

    if (linker == LinkerType::AUTO) {
        if (system("which gcc >/dev/null 2>&1") == 0)        linker = LinkerType::GCC;
        else if (system("which clang >/dev/null 2>&1") == 0) linker = LinkerType::CLANG;
        else
            Err("No suitable linker found (install gcc or clang)");
    }

    switch (linker) {
        case LinkerType::AUTO:
        case LinkerType::GCC:   cmd.push_back("gcc");   break;
        case LinkerType::CLANG: cmd.push_back("clang"); break;
    }

    cmd.push_back("-o");
    cmd.push_back(opts.output_name);
    
    for (const auto& obj  : opts.object_files)  cmd.push_back(obj);
    for (const auto& path : opts.library_paths) cmd.push_back("-L" + path);
    for (const auto& lib  : opts.libraries)     cmd.push_back("-l" + lib);

    if (static_link) cmd.push_back("-static");
    if (strip_syms)  cmd.push_back("-s");

    str command;
    for (std::size_t i = 0; i < cmd.size(); ++i) {
        if (i > 0) command += " ";

        if (cmd[i].find(' ') != str::npos) 
            command += "\"" + cmd[i] + "\"";
        else
            command += cmd[i];
    }

    if (verbose) std::cerr << "Linking: " << command << std::endl;

    int result = system(command.c_str());
    if (result != 0) return Err("Linker failed with exit code: " + std::to_string(result));

    return {};
}


LinkOptions build_link_options(const str& output_name, const vec<str>& object_files) {

    LinkOptions opts {
        .output_name    = output_name,
        .object_files   = object_files,
        .static_link    = static_link.getValue(),
        .strip_symbols  = strip_syms.getValue(),
        .verbose        = verbose.getValue(),
    };

    if      (linker_name.getValue() == "auto")  opts.linker = LinkerType::AUTO;
    else if (linker_name.getValue() == "gcc")   opts.linker = LinkerType::GCC;
    else if (linker_name.getValue() == "clang") opts.linker = LinkerType::CLANG;
    else
        std::cerr << "Unknown linker: " << linker_name.getValue() << ", using auto-detection\n";


    for (const auto& lib : libraries) opts.libraries.push_back(lib);
    for (const auto& path : lib_paths) opts.library_paths.push_back(path);

    // Always link with libc
    if (std::find(opts.libraries.begin(), opts.libraries.end(), "c") == opts.libraries.end()) {
        opts.libraries.push_back("c");
    }

    return opts;
}
