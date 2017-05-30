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

#define ISTYPE(value, id) (value->getType()->getTypeID() == id)

/*
 * TODO: 1. for, while loop
 *       2. obj code
 *       3. struct define
 *
 *
 */
static string llvmTypeToStr(Value* value){
    Type::TypeID typeID;
    if( value )
        typeID = value->getType()->getTypeID();
    else
        return "Value is nullptr";

    switch (typeID){
        case Type::VoidTyID:
            return "VoidTyID";
        case Type::HalfTyID:
            return "HalfTyID";
        case Type::FloatTyID:
            return "FloatTyID";
        case Type::DoubleTyID:
            return "DoubleTyID";
        case Type::IntegerTyID:
            return "IntegerTyID";
        case Type::FunctionTyID:
            return "FunctionTyID";
        case Type::StructTyID:
            return "StructTyID";
        case Type::ArrayTyID:
            return "ArrayTyID";
        case Type::PointerTyID:
            return "PointerTyID";
        case Type::VectorTyID:
            return "VectorTyID";
        default:
            return "Unknown";
    }
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

Value* CastToBoolean(CodeGenContext& context, Value* condValue){

    if( ISTYPE(condValue, Type::IntegerTyID) ){
        condValue = context.builder.CreateIntCast(condValue, Type::getInt1Ty(getGlobalContext()), true);
        return context.builder.CreateICmpNE(condValue, ConstantInt::get(Type::getInt1Ty(getGlobalContext()), 0, true));
    }else if( ISTYPE(condValue, Type::DoubleTyID) ){
        return context.builder.CreateFCmpONE(condValue, ConstantFP::get(getGlobalContext(), APFloat(0.0)));
    }else{
        return condValue;
//        return LogErrorV("Invalid condition type");
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
    Value* dst = context.getSymbolValue(this->lhs.name);
    string dstType = context.getSymbolType(this->lhs.name);
    if( !dst ){
        return LogErrorV("Undeclared variable");
    }
    Value* exp = exp = this->rhs.codeGen(context);

    if( dstType == "int" ){            // since dst.llvm::type is pointerTy
        if( ISTYPE(exp, Type::DoubleTyID) )
            exp = context.builder.CreateFPToUI(exp, Type::getInt64Ty(getGlobalContext()));
        else if( ISTYPE(exp, Type::IntegerTyID) )
            exp = context.builder.CreateIntCast(exp, Type::getInt64Ty(getGlobalContext()), true);
        else
            return LogErrorV("TODO");
    }else if( dstType == "double" && ISTYPE(exp, Type::IntegerTyID) ){
        exp = context.builder.CreateUIToFP(exp, Type::getDoubleTy(getGlobalContext()));
    }

    cout << "dst typeid = " << llvmTypeToStr(dst) << endl;
    cout << "exp typeid = " << llvmTypeToStr(exp) << ":" << (exp->getType()->getTypeID() == Type::IntegerTyID) << endl;

//    lhs.print("lhs: ");
//    rhs.print("rhs: ");
    return context.builder.CreateStore(exp, dst);
//    return new StoreInst(exp, dst, false, context.currentBlock());
}

llvm::Value* NBinaryOperator::codeGen(CodeGenContext &context) {
    cout << "Generating binary operator" << endl;

    Value* L = this->lhs.codeGen(context);
    Value* R = this->rhs.codeGen(context);
    bool fp = false;

    if( (L->getType()->getTypeID() == Type::DoubleTyID) || (R->getType()->getTypeID() == Type::DoubleTyID) ){  // type upgrade
        fp = true;
        if( (R->getType()->getTypeID() != Type::DoubleTyID) ){
            R = context.builder.CreateUIToFP(R, Type::getDoubleTy(getGlobalContext()), "ftmp");
        }
        if( (L->getType()->getTypeID() != Type::DoubleTyID) ){
            L = context.builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()), "ftmp");
        }
    }

    if( !L || !R ){
        return nullptr;
    }
    cout << "fp = " << ( fp ? "true" : "false" ) << endl;
    cout << "L is " << llvmTypeToStr(L) << endl;
    cout << "R is " << llvmTypeToStr(R) << endl;


    switch (this->op){
        case TPLUS:
            return fp ? context.builder.CreateFAdd(L, R, "addftmp") : context.builder.CreateAdd(L, R, "addtmp");
        case TMINUS:
            return fp ? context.builder.CreateFSub(L, R, "subftmp") : context.builder.CreateSub(L, R, "subtmp");
        case TMUL:
            return fp ? context.builder.CreateFMul(L, R, "mulftmp") : context.builder.CreateMul(L, R, "multmp");
        case TCLT:
            return fp ? context.builder.CreateFCmpULT(L, R, "cmpftmp") : context.builder.CreateICmpULT(L, R, "cmptmp");
        case TCLE:
            return fp ? context.builder.CreateFCmpOLE(L, R, "cmpftmp") : context.builder.CreateICmpSLE(L, R, "cmptmp");
        case TCGE:
            return fp ? context.builder.CreateFCmpOGE(L, R, "cmpftmp") : context.builder.CreateICmpSGE(L, R, "cmptmp");
        case TCGT:
            return fp ? context.builder.CreateFCmpOGT(L, R, "cmpftmp") : context.builder.CreateICmpSGT(L, R, "cmptmp");
        case TCEQ:
            return fp ? context.builder.CreateFCmpOEQ(L, R, "cmpftmp") : context.builder.CreateICmpEQ(L, R, "cmptmp");
        case TCNE:
            return fp ? context.builder.CreateFCmpONE(L, R, "cmpftmp") : context.builder.CreateICmpNE(L, R, "cmptmp");
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
    Value* value = context.getSymbolValue(this->name);
    if( !value ){
        LogErrorV("Unknown variable name " + this->name);
    }
//    return new LoadInst(context.locals()[this->name], "", false, context.currentBlock());
    return context.builder.CreateLoad(value, false, "");
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
//    context.types()[this->id.name] = this->type.name;

    context.setSymbolType(this->id.name, this->type.name);
    context.setSymbolValue(this->id.name, inst);
    context.PrintSymTable();
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

llvm::Value* NIfStatement::codeGen(CodeGenContext &context) {
    cout << "Generating if statement" << endl;
    Value* condValue = this->condition.codeGen(context);
    if( !condValue )
        return nullptr;

    condValue = CastToBoolean(context, condValue);

    Function* theFunction = context.builder.GetInsertBlock()->getParent();      // the function where if statement is in

    BasicBlock *thenBB = BasicBlock::Create(getGlobalContext(), "then", theFunction);
    BasicBlock *falseBB = BasicBlock::Create(getGlobalContext(), "else");
    BasicBlock *mergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");

    if( this->falseBlock ){
        context.builder.CreateCondBr(condValue, thenBB, falseBB);
    } else{
        context.builder.CreateCondBr(condValue, thenBB, mergeBB);
    }

    context.builder.SetInsertPoint(thenBB);

    context.pushBlock(thenBB);

    this->trueBlock->codeGen(context);

    context.popBlock();

    thenBB = context.builder.GetInsertBlock();

    if( thenBB->getTerminator() == nullptr ){       //
        context.builder.CreateBr(mergeBB);
    }

    if( this->falseBlock ){
        theFunction->getBasicBlockList().push_back(falseBB);    //
        context.builder.SetInsertPoint(falseBB);            //

        context.pushBlock(thenBB);

        this->falseBlock->codeGen(context);

        context.popBlock();

        context.builder.CreateBr(mergeBB);
    }

    theFunction->getBasicBlockList().push_back(mergeBB);        //
    context.builder.SetInsertPoint(mergeBB);        //

    return nullptr;
}

llvm::Value* NForStatement::codeGen(CodeGenContext &context) {

    Value* condValue = this->condition->codeGen(context);
    if( !condValue )
        return nullptr;

    condValue = CastToBoolean(context, condValue);

    Function* theFunction = context.builder.GetInsertBlock()->getParent();

    BasicBlock *block = BasicBlock::Create(getGlobalContext(), "forloop", theFunction);
    BasicBlock *after = BasicBlock::Create(getGlobalContext(), "forcont");

    // execute the initial
    if( this->initial )
        this->initial->codeGen(context);

    // fall to the block
    context.builder.CreateCondBr(condValue, block, after);

    context.builder.SetInsertPoint(block);

    context.pushBlock(block);

    this->block.codeGen(context);

    context.popBlock();

    // do increment
    if( this->increment ){
        this->increment->codeGen(context);
    }
//    Value* var = context.builder.CreateLoad(counter);
//    Value* result = context.builder.CreateAdd(var, this->increment->codeGen(context), "counter");
//    context.builder.CreateStore(result, counter);

    // execute the again or stop
    condValue = this->condition->codeGen(context);
    condValue = CastToBoolean(context, condValue);
    context.builder.CreateCondBr(condValue, block, after);

    // insert the after block
    theFunction->getBasicBlockList().push_back(after);
    context.builder.SetInsertPoint(after);

    return nullptr;
}



/*
 * Global Functions
 *
 */


std::unique_ptr<NExpression> LogError(const char *str) {
    fprintf(stderr, "LogError: %s\n", str);
    return nullptr;
}

Value *LogErrorV(string str){
    return LogErrorV(str.c_str());
}

Value *LogErrorV(const char *str) {
    LogError(str);
    return nullptr;
}
