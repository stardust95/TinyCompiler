//
// Created by cs on 2017/5/28.
//

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "CodeGen.h"

llvm::Value* NAssignment::codeGen(CodeGenContext &context) {

}

llvm::Value* NBinaryOperator::codeGen(CodeGenContext &context) {
    Value* L = this->lhs.codeGen(context);
    Value* R = this->rhs.codeGen(context);
    if( !L || !R ){
        return nullptr;
    }
    switch (this->op){
        case '+':
            return context.Builder.CreateAdd(L, R, "addtmp");

        case '-':
            return context.Builder.CreateSub(L, R, "subtmp");

        case '*':
            return context.Builder.CreateMul(L, R, "multmp");

        case '<':
            return context.Builder.Create(L, R, "cmptmp");

        default:
            return LogErrorV("Unknown binary operator");
    }

}

llvm::Value* NBlock::codeGen(CodeGenContext &context) {

}

llvm::Value* NDouble::codeGen(CodeGenContext &context) {

}

llvm::Value* NExpressionStatement::codeGen(CodeGenContext &context) {

}

llvm::Value* NFunctionDeclaration::codeGen(CodeGenContext &context) {

}

llvm::Value* NIdentifier::codeGen(CodeGenContext &context) {
    Value* v = context.nameValues[this->name];
    if( !v ){
        LogErrorV("Unknown variable name");
    }
    return v;
}

llvm::Value* NInteger::codeGen(CodeGenContext &context) {
    return ConstantInt::get(context.llvmContext, APInt(INTBITS, this->value, true));
}

llvm::Value* NMethodCall::codeGen(CodeGenContext &context) {

}

llvm::Value* NVariableDeclaration::codeGen(CodeGenContext &context) {

}

Value *LogErrorV(const char *str) {
    LogError(str);
    return nullptr;
}
