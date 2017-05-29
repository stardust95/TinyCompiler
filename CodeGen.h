#ifndef __CODEGEN_H__
#define __CODEGEN_H__


#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <stack>
#include <memory>
#include <string>
#include <map>
#include "ASTNodes.h"
#include "grammar.hpp"

using namespace llvm;
using std::unique_ptr;
using std::string;

static const int INTBITS = 64;

using SymTable = std::map<string, Value*>;

class CodeGenBlock{
public:
    BasicBlock * block;
    Value * returnValue;
    std::map<string, Value*> locals;
};

class CodeGenContext{
private:
    std::stack<CodeGenBlock*> blockStack;

public:
    LLVMContext llvmContext;
    IRBuilder<> builder;
    unique_ptr<Module> theModule;
    SymTable globalVars;

    CodeGenContext(): builder(llvmContext){
        theModule = unique_ptr<Module>(new Module("main", getGlobalContext()));
    }

    SymTable& locals() const{
        return blockStack.top()->locals;
    };

    BasicBlock* currentBlock() const{
        return blockStack.top()->block;
    }

    void pushBlock(BasicBlock * block){
        CodeGenBlock * codeGenBlock = new CodeGenBlock();
        codeGenBlock->block = block;
        codeGenBlock->returnValue = nullptr;
        blockStack.push(codeGenBlock);
    }

    void popBlock(){
        CodeGenBlock * codeGenBlock = blockStack.top();
        blockStack.pop();
        delete codeGenBlock;
    }

    void setCurrentReturnValue(Value* value){
        blockStack.top()->returnValue = value;
    }

    Value* getCurrentReturnValue(){
        return blockStack.top()->returnValue;
    }

    void generateCode(NBlock& );

    void runCode();


};

Value* LogErrorV(const char* str);

//llvm::Value* NAssignment::codeGen(CodeGenContext &context) ;
//
//llvm::Value* NBinaryOperator::codeGen(CodeGenContext &context) ;
//
//llvm::Value* NBlock::codeGen(CodeGenContext &context) ;
//
//llvm::Value* NDouble::codeGen(CodeGenContext &context) ;
//
//llvm::Value* NExpressionStatement::codeGen(CodeGenContext &context) ;
//
//llvm::Value* NFunctionDeclaration::codeGen(CodeGenContext &context) ;
//
//llvm::Value* NIdentifier::codeGen(CodeGenContext &context) ;
//
//llvm::Value* NInteger::codeGen(CodeGenContext &context) ;
//
//llvm::Value* NMethodCall::codeGen(CodeGenContext &context) ;
//
//llvm::Value* NVariableDeclaration::codeGen(CodeGenContext &context) ;

#endif