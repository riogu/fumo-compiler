#include <llvm/Passes/PassBuilder.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/MC/TargetRegistry.h>
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
    // NOTE: this is here so i can debug the compiler, later these should be deleted 
    //        and only generated at the end of the function
    if (print_IR) {
        fs::path output_name = llvm_module->getModuleIdentifier();
        output_name.replace_extension(".ll");
        std::cerr << "llvm IR for '" << output_name.string() << "':\n" << llvm_ir_to_str() << std::endl;
    }
    if (print_AST) {
        std::cerr << "debug AST for '" << llvm_module->getSourceFileName() << "':\n";
        for (const auto& node : get<NamespaceDecl>(file_root_node).nodes)
            std::cerr << "node found:\n  " + node->to_str() + "\n";
    }

    if (llvm::verifyModule(*llvm_module, &llvm::WithColor::error(llvm::errs()))) {
        std::cerr << '\n';
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

    // llvm::PassBuilder passBuilder;
    // llvm::ModuleAnalysisManager mam;
    // llvm::LoopAnalysisManager lam;
    // llvm::FunctionAnalysisManager fam;
    // llvm::CGSCCAnalysisManager cgam;
    // passBuilder.registerModuleAnalyses(mam);
    // passBuilder.registerCGSCCAnalyses(cgam);
    // passBuilder.registerFunctionAnalyses(fam);
    // passBuilder.registerLoopAnalyses(lam);
    // passBuilder.crossRegisterProxies(lam, fam, cgam, mam);
    //
    // llvm::ModulePassManager mpm;
    // if (opt_level != llvm::OptimizationLevel::O0) {
    //     mpm = passBuilder.buildPerModuleDefaultPipeline(opt_level);
    // } else {
    //     mpm = passBuilder.buildO0DefaultPipeline(opt_level);
    // }
    // mpm.run(*llvm_module, mam);
    
    auto Features = "";
    auto CPU = "generic";
    llvm::TargetOptions opt;    
    auto target_machine = target->createTargetMachine(llvm_module->getTargetTriple(), CPU, Features, opt, llvm::Reloc::PIC_);

    llvm_module->setDataLayout(target_machine->createDataLayout());
    llvm::legacy::PassManager pass;
    target_machine->Options.MCOptions.AsmVerbose = true;



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

        if (target_machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::AssemblyFile)) {
            INTERNAL_PANIC("Target does not support emission of assembly files.");
        }
        pass.run(*llvm_module);
    }
    if (output_OBJ || !emit_flag_was_set) {
        dest_file_name.replace_extension(".o");
        llvm::raw_fd_ostream dest(dest_file_name.string(), EC, llvm::sys::fs::OF_None);

        if (target_machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
            INTERNAL_PANIC("Target does not support emission of object files.");
        }
        pass.run(*llvm_module);
    }

    if (print_ASM) INTERNAL_PANIC("[TODO] printing ASM isn't implemented yet.");

    if (print_file) {
        std::cerr << "file contents for '" << llvm_module->getSourceFileName() << "':\n"
                  << file_stream.str() << std::endl;
    }
    if (print_IR) {
        fs::path output_name = llvm_module->getModuleIdentifier();
        output_name.replace_extension(".ll");
        std::cerr << "llvm IR for '" << output_name.string() << "':\n" << llvm_ir_to_str() << std::endl;
    }
    if (print_AST) {
        std::cerr << "debug AST for '" << llvm_module->getSourceFileName() << "':\n";
        for (const auto& node : get<NamespaceDecl>(file_root_node).nodes)
            std::cerr << "node found:\n  " + node->to_str() + "\n";
    }
}
