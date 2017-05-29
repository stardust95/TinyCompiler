
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <

#include <memory>
#include <string>
#include <map>
#include "ASTNodes.h"
#include "CodeGen.h"

using namespace llvm;
using std::unique_ptr;
using std::string;

static const int INTBITS = 64;

class CodeGenContext{
public:
    LLVMContext llvmContext;
    IRBuilder<> Builder;
    unique_ptr<Module> theModule;
    std::map<string, Value*> nameValues;


};

Value* LogErrorV(const char* str);

llvm::Value* NAssignment::codeGen(CodeGenContext &context) ;

llvm::Value* NBinaryOperator::codeGen(CodeGenContext &context) ;

llvm::Value* NBlock::codeGen(CodeGenContext &context) ;

llvm::Value* NDouble::codeGen(CodeGenContext &context) ;

llvm::Value* NExpressionStatement::codeGen(CodeGenContext &context) ;

llvm::Value* NFunctionDeclaration::codeGen(CodeGenContext &context) ;

llvm::Value* NIdentifier::codeGen(CodeGenContext &context) ;

llvm::Value* NInteger::codeGen(CodeGenContext &context) ;

llvm::Value* NMethodCall::codeGen(CodeGenContext &context) ;

llvm::Value* NVariableDeclaration::codeGen(CodeGenContext &context) ;

