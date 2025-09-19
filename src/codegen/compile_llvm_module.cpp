#include <llvm/Passes/PassBuilder.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/WithColor.h>
#include "codegen/llvm_codegen.hpp"
#include "utils/common_utils.hpp"

void Codegen::compile_module(llvm::OptimizationLevel opt_level) {

    // clear_metadata(); // NOTE: add this if you are having issues with corrupted debug metadata

    //------------------------------------------------------------------------------------------
    // initialization, checking moule
    std::error_code EC;
    fs::path source_file_name = file.path_name;

    std::string error_buffer;
    llvm::raw_string_ostream error_stream(error_buffer);

    if (llvm::verifyModule(*llvm_module, &llvm::WithColor::error(error_stream))) {
        source_file_name.replace_extension(".fm");
        std::cerr << "\ninput file '" << llvm_module->getSourceFileName() << "':\n" <<  file.contents << std::endl;
        source_file_name.replace_extension(".ll");
        std::cerr << "\nllvm IR for '" << source_file_name.string() << "':\n" << llvm_ir_to_str() << std::endl;
        error_stream.flush();
        std::cerr << error_buffer << '\n';
        internal_panic("found error while generating llvm IR.");
    }

    if (print_file) {
        source_file_name.replace_extension(".fm");
        std::cerr << "\ninput file '" << llvm_module->getSourceFileName() << "':\n" <<  file.contents << std::endl;
    }
    if (print_AST) {
        std::cerr << "\ndebug AST for '" << llvm_module->getSourceFileName() << "':\n";
        for (const auto& node : get<NamespaceDecl>(file_root_node).nodes) {
            std::cerr << "node found:\n  " + node->to_str() + "\n";
        }
    }
    if (output_IR_O0) { // NOTE: this is here for debugging and comparing to the optimized IR
        fs::path name_copy = source_file_name.parent_path() / source_file_name.stem(); name_copy += "-O0.ll";
        llvm::raw_fd_ostream dest(name_copy.string(), EC);
        dest << llvm_ir_to_str();
        // std::cerr << "\nllvm IR for '" << name_copy << "':\n" << llvm_ir_to_str() << std::endl;
    }
    if (print_IR) {
        source_file_name.replace_extension(".ll");
        std::cerr << "\nllvm IR for '" << source_file_name.string() << "':\n" << llvm_ir_to_str() << std::endl;
    }

    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    llvm_module->setTargetTriple(llvm::sys::getDefaultTargetTriple());
    std::string Error;
    auto target = llvm::TargetRegistry::lookupTarget(llvm_module->getTargetTriple(), Error);
    if (!target) internal_panic("{}", Error);

    auto Features = "";
    auto CPU = "generic";
    llvm::TargetOptions opt;    
    auto target_machine = target->createTargetMachine(llvm_module->getTargetTriple(), 
                                                      CPU, Features, opt, llvm::Reloc::PIC_);
    if (!target_machine) internal_panic("Failed to create target machine");

    llvm_module->setDataLayout(target_machine->createDataLayout());

    
    //------------------------------------------------------------------------------------------
    // optimization passes
    llvm::PassBuilder pass_builder(target_machine);

    llvm::FunctionAnalysisManager fam;
    llvm::LoopAnalysisManager lam;
    llvm::CGSCCAnalysisManager cgam;
    llvm::ModuleAnalysisManager mam;

    pass_builder.registerModuleAnalyses(mam);
    pass_builder.registerCGSCCAnalyses(cgam);
    pass_builder.registerFunctionAnalyses(fam);
    pass_builder.registerLoopAnalyses(lam);
    pass_builder.crossRegisterProxies(lam, fam, cgam, mam);

    
    llvm::ModulePassManager mpm;
    switch (opt_level.getSpeedupLevel()) {
        case 0:  mpm = pass_builder.buildO0DefaultPipeline(opt_level); break;
        case 1:  mpm = pass_builder.buildPerModuleDefaultPipeline(opt_level); break;
        case 2:  mpm = pass_builder.buildPerModuleDefaultPipeline(opt_level); break;
        case 3:  mpm = pass_builder.buildPerModuleDefaultPipeline(opt_level); break;
        default: mpm = pass_builder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2); break;
    }
    mpm.run(*llvm_module, mam);

    //------------------------------------------------------------------------------------------
    // outputting, printing (result of the recieved compiler flags)
    bool emit_flag_was_set = false;

    if (output_AST) {
        emit_flag_was_set = true;
        source_file_name.replace_extension(".ast");
        llvm::raw_fd_ostream dest(source_file_name.string(), EC);
        dest << "debug AST for '" << llvm_module->getSourceFileName() << "':\n";
        for (const auto& node : get<NamespaceDecl>(file_root_node).nodes)
            dest << "node found:\n  " + node->to_str() + "\n";
    }
    if (output_IR) {
        emit_flag_was_set = true;
        source_file_name.replace_extension(".ll");
        llvm::raw_fd_ostream dest(source_file_name.string(), EC);
        dest << llvm_ir_to_str();
    }
    if (output_ASM || print_ASM) {
        emit_flag_was_set = true;
        source_file_name.replace_extension(".s");

        llvm::SmallVector<char, 0> asm_buffer; llvm::raw_svector_ostream asm_stream(asm_buffer);

        llvm::legacy::PassManager pass;
        target_machine->Options.MCOptions.AsmVerbose = true;
        if (output_ASM &&
            target_machine->addPassesToEmitFile(pass, asm_stream, nullptr, llvm::CodeGenFileType::AssemblyFile)) {
            internal_panic("Target does not support emission of assembly files.");
        }
        pass.run(*llvm_module);

        llvm::raw_fd_ostream dest(source_file_name.string(), EC, llvm::sys::fs::OF_None);
        dest.write(asm_buffer.data(), asm_buffer.size());

        if (print_ASM) {
            std::cerr << "\nASM for '" << source_file_name.string()  << "':\n"
                      << str(asm_buffer.data(), asm_buffer.size()) << std::endl;
        }
    }

    if(emit_flag_was_set){}

    source_file_name.replace_extension(".o");
    llvm::raw_fd_ostream dest(source_file_name.string(), EC, llvm::sys::fs::OF_None);

    llvm::legacy::PassManager pass;
    if (target_machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
        internal_panic("Target does not support emission of object files.");
    }
    pass.run(*llvm_module);
    
}


[[nodiscard]] fs::path Codegen::compile_file(llvm::OptimizationLevel opt_level) {
    
    compile_module(opt_level);
    auto obj_file_name = file.path_name;
    obj_file_name.replace_extension(".o");
    
    if (!fs::exists(obj_file_name)) {
        std::cerr << "Object file not found: " << obj_file_name << std::endl;
        std::exit(1);
    }

    return obj_file_name;
}

void link_object_files(vec<str> obj_files, fs::path exec_name) {


    // only compiling a single object file for now

    exec_name.replace_extension("");

    LinkOptions link_opts = build_link_options(exec_name, obj_files);

    if (auto result = link_executable(link_opts)) {
        if (verbose) std::cout << "Successfully created executable: " << exec_name << std::endl;
        if (!output_OBJ) {
            for (const auto& obj_file : obj_files) fs::remove(obj_file);
        }
    } else {
        std::cerr << "Linking failed: " << result.error() << std::endl;
    }
}
