#include <fstream>
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

void Codegen::compile_module(llvm::OptimizationLevel opt_level) {
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

    llvm::PassBuilder passBuilder;
    llvm::ModuleAnalysisManager mam;
    llvm::LoopAnalysisManager lam;
    llvm::FunctionAnalysisManager fam;
    llvm::CGSCCAnalysisManager cgam;
    passBuilder.registerModuleAnalyses(mam);
    passBuilder.registerCGSCCAnalyses(cgam);
    passBuilder.registerFunctionAnalyses(fam);
    passBuilder.registerLoopAnalyses(lam);
    passBuilder.crossRegisterProxies(lam, fam, cgam, mam);

    llvm::ModulePassManager mpm;
    if (opt_level != llvm::OptimizationLevel::O0) {
        mpm = passBuilder.buildPerModuleDefaultPipeline(opt_level);
    } else {
        mpm = passBuilder.buildO0DefaultPipeline(opt_level);
    }
    mpm.run(*llvm_module, mam);
    
    auto Features = "";
    auto CPU = "generic";
    llvm::TargetOptions opt;    
    auto target_machine = target->createTargetMachine(llvm_module->getTargetTriple(), CPU, Features, opt, llvm::Reloc::PIC_);

    llvm_module->setDataLayout(target_machine->createDataLayout());
    llvm::legacy::PassManager pass;
    target_machine->Options.MCOptions.AsmVerbose = true;

    fs::path dest_file_name = llvm_module->getName().str();
    dest_file_name.replace_extension(".asm");

    std::error_code EC;
    llvm::raw_fd_ostream dest(dest_file_name.string(), EC, llvm::sys::fs::OF_None);

    if (target_machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::AssemblyFile)) {
        INTERNAL_PANIC("Target does not support emission of object files.");
    }

    // pass.run(*llvm_module);
    // dest.flush();
}
