//
// Created by cs on 2017/5/28.
//

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/Support/raw_ostream.h>

#include "CodeGen.h"
#include "ASTNodes.h"

#define ISTYPE(ty1,ty2) (typeid(ty1) == typeid(ty2))

/*
 * TODO: 1. type upgrade in assign
 *
 *
 */


static void PrintSymTable(SymTable table){
    cout << "======= Print Symbol Table ========" << endl;
    for(auto it=table.begin(); it!=table.end(); it++){
        cout << it->first << " = " << it->second << endl;
    }
    cout << "===================================" << endl;
}

static Type* TypeOf(const NIdentifier & type){        // get llvm::type of variable base on its identifier
    cout << "TypeOf " << type.name << endl;
    if( type.name.compare("int") == 0 ){
        return Type::getInt64Ty(getGlobalContext());
    }else if( type.name.compare("double") == 0 ){
        return Type::getDoubleTy(getGlobalContext());
    }else{
        return Type::getVoidTy(getGlobalContext());
    }
}

void CodeGenContext::generateCode(NBlock& root) {
    cout << "Generating IR code" << endl;

    std::vector<Type*> sysArgs;
    FunctionType* mainFuncType = FunctionType::get(Type::getVoidTy(getGlobalContext()), makeArrayRef(sysArgs), false);
    Function* mainFunc = Function::Create(mainFuncType, GlobalValue::InternalLinkage, "main");
    BasicBlock* block = BasicBlock::Create(getGlobalContext(), "entry");

    pushBlock(block);
    Value* retValue = root.codeGen(*this);
    popBlock();

    cout << "Code generate success" << endl;

    PassManager passManager;
    passManager.add(createPrintModulePass(outs()));
    passManager.run(*(this->theModule.get()));
    return;
}

llvm::Value* NAssignment::codeGen(CodeGenContext &context) {
    cout << "Generating assignment of " << this->lhs.name << " = " << endl;
    if( context.locals().find(this->lhs.name) == context.locals().end() ){
        return LogErrorV("Undeclared variable");
    }
    Value* dst = context.locals()[this->lhs.name];
    Value* exp = nullptr;
    string type = context.types()[this->lhs.name];

    // TODO
//    if( type == "int" && ISTYPE(rhs, NDouble) ){
//        exp = (NInteger(dynamic_cast<NDouble&>(rhs).value)).codeGen(context);
//    }else if( type == "double" && ISTYPE(rhs, NInteger) ){
//        exp = ((NDouble) dynamic_cast<NInteger&>(rhs)).codeGen(context);
//    }
    if( exp == nullptr ){
        exp = this->rhs.codeGen(context);
    }
    cout << "type = " << type << endl;
    lhs.print("lhs: ");
    rhs.print("rhs: ");
    return context.builder.CreateStore(exp, dst);
//    return new StoreInst(exp, dst, false, context.currentBlock());
}

llvm::Value* NBinaryOperator::codeGen(CodeGenContext &context) {
    cout << "Generating binary operator" << endl;

    Value* L = this->lhs.codeGen(context);
    Value* R = this->rhs.codeGen(context);
    bool fp = false;

    if( ISTYPE(this->rhs, NDouble) || ISTYPE(this->lhs, NDouble) ){  // type upgrade
        fp = true;
        if( ISTYPE(this->rhs, NInteger) ){
            R = ((NDouble)dynamic_cast<NInteger&>(this->rhs)).codeGen(context);
        }
        if( ISTYPE(this->lhs, NInteger) ){
            L = ((NDouble) dynamic_cast<NInteger&>(this->lhs)).codeGen(context);
        }
    }

    if( !L || !R ){
        return nullptr;
    }

    cout << "fp is " << (fp ? "true" : "false") << endl;

    switch (this->op){
        case TPLUS:
            return fp ? context.builder.CreateFAdd(L, R, "addftmp") : context.builder.CreateAdd(L, R, "addtmp");

        case TMINUS:
            return fp ? context.builder.CreateFSub(L, R, "subftmp") : context.builder.CreateSub(L, R, "subtmp");

        case TMUL:
            return fp ? context.builder.CreateFMul(L, R, "mulftmp") : context.builder.CreateMul(L, R, "multmp");

        // TODO： compare codegen
//        case '<':
//            return context.Builder.Create(L, R, "cmptmp");
        default:
            return LogErrorV("Unknown binary operator");
    }

}

llvm::Value* NBlock::codeGen(CodeGenContext &context) {
    cout << "Generating block" << endl;
    Value* last = nullptr;
    for(auto it=this->statements.begin(); it!=this->statements.end(); it++){
        last = (*it)->codeGen(context);
    }
    return last;
}

llvm::Value* NInteger::codeGen(CodeGenContext &context) {
    cout << "Generating Integer: " << this->value << endl;
    return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), this->value, true);
//    return ConstantInt::get(getGlobalContext(), APInt(INTBITS, this->value, true));
}

llvm::Value* NDouble::codeGen(CodeGenContext &context) {
    cout << "Generating Double: " << this->value << endl;
    return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), this->value);
//    return ConstantFP::get(getGlobalContext(), APFloat(this->value));
}

llvm::Value* NIdentifier::codeGen(CodeGenContext &context) {
    cout << "Generating identifier " << this->name << endl;
    if( context.locals().find(this->name) == context.locals().end() ){
        LogErrorV("Unknown variable name");
    }
//    return new LoadInst(context.locals()[this->name], "", false, context.currentBlock());
    return context.builder.CreateLoad(context.locals()[this->name], false, "");
}

llvm::Value* NExpressionStatement::codeGen(CodeGenContext &context) {
    return this->expression.codeGen(context);
}

llvm::Value* NFunctionDeclaration::codeGen(CodeGenContext &context) {
    cout << "Generating function declaration of " << this->id.name << endl;
    std::vector<Type*> argTypes;

    for(auto &arg: this->arguments){
        argTypes.push_back(TypeOf(arg->type));
    }
    FunctionType* functionType = FunctionType::get(TypeOf(this->type), argTypes, false);
    Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, this->id.name.c_str(), context.theModule.get());
    BasicBlock* basicBlock = BasicBlock::Create(getGlobalContext(), "entry", function, nullptr);

    context.builder.SetInsertPoint(basicBlock);
    context.pushBlock(basicBlock);

    // declare function params
    Function::arg_iterator ir_arg_it = function->arg_begin();
    Value* ir_arg;

    for(auto& origin_arg_it: this->arguments){
        Value* argAlloc = origin_arg_it->codeGen(context);

        ir_arg = ir_arg_it++;
        ir_arg->setName(origin_arg_it->id.name);
        context.builder.CreateStore(ir_arg, argAlloc, false);
    }

    this->block.codeGen(context);

    if( context.getCurrentReturnValue() ){
        context.builder.CreateRet(context.getCurrentReturnValue());
    } else{
        return LogErrorV("Function block return value not founded");
    }

    // TODO: function parameter variables?
    context.popBlock();

    return function;
}


llvm::Value* NMethodCall::codeGen(CodeGenContext &context) {
    cout << "Generating method call of " << this->id.name << endl;
    Function * calleeF = context.theModule->getFunction(this->id.name);
    if( !calleeF ){
        LogErrorV("Function name not found");
    }
    if( calleeF->arg_size() != this->arguments.size() ){
        LogErrorV("Function arguments size not match");
    }
    std::vector<Value*> argsv;
    for(auto it=this->arguments.begin(); it!=this->arguments.end(); it++){
        argsv.push_back((*it)->codeGen(context));
        if( !argsv.back() ){        // if any argument codegen fail
            return nullptr;
        }
    }
    return context.builder.CreateCall(calleeF, argsv, "calltmp");
}

llvm::Value* NVariableDeclaration::codeGen(CodeGenContext &context) {
    cout << "Generating variable declaration of " << this->type.name << " " << this->id.name << endl;
    Type* type = TypeOf(this->type);

    AllocaInst* inst = context.builder.CreateAlloca(type);
    context.types()[this->id.name] = this->type.name;

    context.locals()[this->id.name] = inst;
    PrintSymTable(context.locals());
    if( this->assignmentExpr ){
        NAssignment assignment(this->id, *(this->assignmentExpr));
        assignment.codeGen(context);
    }
    return inst;
}

llvm::Value* NReturnStatement::codeGen(CodeGenContext &context) {
    cout << "Generating return statement" << endl;
    Value* returnValue = this->expression.codeGen(context);
    context.setCurrentReturnValue(returnValue);
    return returnValue;
}

std::unique_ptr<NExpression> LogError(const char *str) {
    fprintf(stderr, "LogError: %s\n", str);
    return nullptr;
}

Value *LogErrorV(const char *str) {
    LogError(str);
    return nullptr;
}
