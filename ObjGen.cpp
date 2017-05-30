//
// Created by cs on 2017/5/30.
//
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/ADT/Optional.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/PassManager.h>

#include "CodeGen.h"
#include "ObjGen.h"

using namespace llvm;


void ObjGen(CodeGenContext & context, const string& filename){
    // Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto targetType = sys::getDefaultTargetTriple();
    context.theModule->setTargetTriple(targetType);

    std::string error;
    auto Target = TargetRegistry::lookupTarget(targetType, error);

    if( !Target ){
        errs() << error;
        return;
    }

    auto CPU = "generic";
    auto features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
//    auto RM = Reloc::Model ();
    auto theTargetMachine = Target->createTargetMachine(targetType, CPU, features, opt, RM);

    context.theModule->setDataLayout(theTargetMachine->createDataLayout());

    std::error_code EC;
    raw_fd_ostream dest(filename.c_str(), EC, sys::fs::F_None);
//    raw_fd_ostream dest(filename.c_str(), EC, sys::fs::F_None);
//    formatted_raw_ostream formattedRawOstream(dest);

    legacy::PassManager pass;
    auto fileType = TargetMachine::CGFT_ObjectFile;

    if( theTargetMachine->addPassesToEmitFile(pass, dest, fileType) ){
        errs() << "theTargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*context.theModule.get());
    dest.flush();

    outs() << "Object code wrote to " << filename.c_str() << "\n";

    return;
}

