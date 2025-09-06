#include <llvm/Passes/PassBuilder.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/WithColor.h>
#include "codegen/llvm_codegen.hpp"

extern llvm::cl::opt<bool> output_IR, output_AST, output_ASM, output_OBJ, 
                           print_file, print_IR, print_AST, print_ASM;

void Codegen::compile_module(llvm::OptimizationLevel opt_level) {

    std::error_code EC;
    fs::path dest_file_name = llvm_module->getModuleIdentifier();

    std::string error_buffer;
    llvm::raw_string_ostream error_stream(error_buffer);
    if (llvm::verifyModule(*llvm_module, &llvm::WithColor::error(error_stream))) {
        error_stream.flush();
        std::cerr << error_buffer << '\n';
        return;
    }

    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    llvm_module->setTargetTriple(llvm::sys::getDefaultTargetTriple());
    std::string Error;
    auto target = llvm::TargetRegistry::lookupTarget(llvm_module->getTargetTriple(), Error);
    if (!target) INTERNAL_PANIC("{}", Error);

    auto Features = "";
    auto CPU = "generic";
    llvm::TargetOptions opt;    
    auto target_machine = target->createTargetMachine(llvm_module->getTargetTriple(), CPU, Features, opt, llvm::Reloc::PIC_);
    if (!target_machine) INTERNAL_PANIC("Failed to create target machine");

    llvm_module->setDataLayout(target_machine->createDataLayout());

    
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
    

    bool emit_flag_was_set = false;
    if (output_AST) {
        emit_flag_was_set = true;
        dest_file_name.replace_extension(".ast");
        llvm::raw_fd_ostream dest(dest_file_name.string(), EC);
        dest << "debug AST for '" << llvm_module->getSourceFileName() << "':\n";
        for (const auto& node : get<NamespaceDecl>(file_root_node).nodes)
            dest << "node found:\n  " + node->to_str() + "\n";
    }
    if (output_IR) {
        emit_flag_was_set = true;
        dest_file_name.replace_extension(".ll");
        llvm::raw_fd_ostream dest(dest_file_name.string(), EC);
        dest << llvm_ir_to_str();
    }
    if (output_ASM) {
        emit_flag_was_set = true;
        dest_file_name.replace_extension(".asm");
        llvm::raw_fd_ostream dest(dest_file_name.string(), EC, llvm::sys::fs::OF_None);

        llvm::legacy::PassManager pass;
        target_machine->Options.MCOptions.AsmVerbose = true;
        if (target_machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::AssemblyFile)) {
            INTERNAL_PANIC("Target does not support emission of assembly files.");
        }
        pass.run(*llvm_module);
    }
    if (output_OBJ || !emit_flag_was_set) {
        dest_file_name.replace_extension(".o");
        llvm::raw_fd_ostream dest(dest_file_name.string(), EC, llvm::sys::fs::OF_None);

        llvm::legacy::PassManager pass;
        if (target_machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
            INTERNAL_PANIC("Target does not support emission of object files.");
        }
        pass.run(*llvm_module);
    }

    if (print_ASM) INTERNAL_PANIC("[TODO] printing ASM isn't implemented yet.");

    if (print_file) {
        std::cerr << "\nfile contents for '" << llvm_module->getSourceFileName() << "':\n"
                  << file_stream.str() << std::endl;
    }
    if (print_IR) {
        fs::path output_name = llvm_module->getModuleIdentifier();
        output_name.replace_extension(".ll");
        std::cerr << "\nllvm IR for '" << output_name.string() << "':\n" << llvm_ir_to_str() << std::endl;
    }
    if (print_AST) {
        std::cerr << "\ndebug AST for '" << llvm_module->getSourceFileName() << "':\n";
        for (const auto& node : get<NamespaceDecl>(file_root_node).nodes)
            std::cerr << "node found:\n  " + node->to_str() + "\n";
    }
}
