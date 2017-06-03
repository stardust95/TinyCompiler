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
//#include <llvm/IR/Verifier.h>
#include "CodeGen.h"
#include "ASTNodes.h"
#include "TypeSystem.h"

#define ISTYPE(value, id) (value->getType()->getTypeID() == id)

/*
 * TODO:
 *       1. unary ops
 *       2. variable declaration list
 *       3. array type as function parameter
 *
 *
 */

//
static Type* TypeOf(const NIdentifier & type, CodeGenContext& context){        // get llvm::type of variable base on its identifier
    return context.typeSystem.getVarType(type);
}

Value* CastToBoolean(CodeGenContext& context, Value* condValue){

    if( ISTYPE(condValue, Type::IntegerTyID) ){
        condValue = context.builder.CreateIntCast(condValue, Type::getInt1Ty(context.llvmContext), true);
        return context.builder.CreateICmpNE(condValue, ConstantInt::get(Type::getInt1Ty(context.llvmContext), 0, true));
    }else if( ISTYPE(condValue, Type::DoubleTyID) ){
        return context.builder.CreateFCmpONE(condValue, ConstantFP::get(context.llvmContext, APFloat(0.0)));
    }else{
        return condValue;
    }
}

void CodeGenContext::generateCode(NBlock& root) {
    cout << "Generating IR code" << endl;

    std::vector<Type*> sysArgs;
    FunctionType* mainFuncType = FunctionType::get(Type::getVoidTy(this->llvmContext), makeArrayRef(sysArgs), false);
    Function* mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main");
    BasicBlock* block = BasicBlock::Create(this->llvmContext, "entry");

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
    cout << "Generating assignment of " << this->lhs->name << " = " << endl;
    Value* dst = context.getSymbolValue(this->lhs->name);
    string dstType = context.getSymbolType(this->lhs->name);
    if( !dst ){
        return LogErrorV("Undeclared variable");
    }
    Value* exp = exp = this->rhs->codeGen(context);

    cout << "dst typeid = " << TypeSystem::llvmTypeToStr(context.typeSystem.getVarType(dstType)) << endl;
    cout << "exp typeid = " << TypeSystem::llvmTypeToStr(exp) << endl;

    exp = context.typeSystem.cast(exp, context.typeSystem.getVarType(dstType), context.currentBlock());
    context.builder.CreateStore(exp, dst);
    return dst;
}

llvm::Value* NBinaryOperator::codeGen(CodeGenContext &context) {
    cout << "Generating binary operator" << endl;

    Value* L = this->lhs->codeGen(context);
    Value* R = this->rhs->codeGen(context);
    bool fp = false;

    if( (L->getType()->getTypeID() == Type::DoubleTyID) || (R->getType()->getTypeID() == Type::DoubleTyID) ){  // type upgrade
        fp = true;
        if( (R->getType()->getTypeID() != Type::DoubleTyID) ){
            R = context.builder.CreateUIToFP(R, Type::getDoubleTy(context.llvmContext), "ftmp");
        }
        if( (L->getType()->getTypeID() != Type::DoubleTyID) ){
            L = context.builder.CreateUIToFP(L, Type::getDoubleTy(context.llvmContext), "ftmp");
        }
    }

    if( !L || !R ){
        return nullptr;
    }
    cout << "fp = " << ( fp ? "true" : "false" ) << endl;
    cout << "L is " << TypeSystem::llvmTypeToStr(L) << endl;
    cout << "R is " << TypeSystem::llvmTypeToStr(R) << endl;

    switch (this->op){
        case TPLUS:
            return fp ? context.builder.CreateFAdd(L, R, "addftmp") : context.builder.CreateAdd(L, R, "addtmp");
        case TMINUS:
            return fp ? context.builder.CreateFSub(L, R, "subftmp") : context.builder.CreateSub(L, R, "subtmp");
        case TMUL:
            return fp ? context.builder.CreateFMul(L, R, "mulftmp") : context.builder.CreateMul(L, R, "multmp");
        case TDIV:
            return fp ? context.builder.CreateFDiv(L, R, "divftmp") : context.builder.CreateSDiv(L, R, "divtmp");
        case TAND:
            return fp ? LogErrorV("Double type has no AND operation") : context.builder.CreateAnd(L, R, "andtmp");
        case TOR:
            return fp ? LogErrorV("Double type has no OR operation") : context.builder.CreateOr(L, R, "ortmp");
        case TXOR:
            return fp ? LogErrorV("Double type has no XOR operation") : context.builder.CreateXor(L, R, "xortmp");
        case TSHIFTL:
            return fp ? LogErrorV("Double type has no LEFT SHIFT operation") : context.builder.CreateShl(L, R, "shltmp");
        case TSHIFTR:
            return fp ? LogErrorV("Double type has no RIGHT SHIFT operation") : context.builder.CreateAShr(L, R, "ashrtmp");

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
    for(auto it=this->statements->begin(); it!=this->statements->end(); it++){
        last = (*it)->codeGen(context);
    }
    return last;
}

llvm::Value* NInteger::codeGen(CodeGenContext &context) {
    cout << "Generating Integer: " << this->value << endl;
    return ConstantInt::get(Type::getInt32Ty(context.llvmContext), this->value, true);
//    return ConstantInt::get(context.llvmContext, APInt(INTBITS, this->value, true));
}

llvm::Value* NDouble::codeGen(CodeGenContext &context) {
    cout << "Generating Double: " << this->value << endl;
    return ConstantFP::get(Type::getDoubleTy(context.llvmContext), this->value);
//    return ConstantFP::get(context.llvmContext, APFloat(this->value));
}

llvm::Value* NIdentifier::codeGen(CodeGenContext &context) {
    cout << "Generating identifier " << this->name << endl;
    Value* value = context.getSymbolValue(this->name);
    if( !value ){
        return LogErrorV("Unknown variable name " + this->name);
    }
    if( value->getType()->isPointerTy() ){
        auto arrayPtr = context.builder.CreateLoad(value, "arrayPtr");
        if( arrayPtr->getType()->isArrayTy() ){
            cout << "(Array Type)" << endl;
//            arrayPtr->setAlignment(16);
            std::vector<Value*> indices;
            indices.push_back(ConstantInt::get(context.typeSystem.intTy, 0, false));
            auto ptr = context.builder.CreateInBoundsGEP(value, indices, "arrayPtr");
            return ptr;
        }
    }
    return context.builder.CreateLoad(value, false, "");

}

llvm::Value* NExpressionStatement::codeGen(CodeGenContext &context) {
    return this->expression->codeGen(context);
}

llvm::Value* NFunctionDeclaration::codeGen(CodeGenContext &context) {
    cout << "Generating function declaration of " << this->id->name << endl;
    std::vector<Type*> argTypes;

    for(auto &arg: *this->arguments){
        if( arg->type->isArray ){
            argTypes.push_back(PointerType::get(context.typeSystem.getVarType(arg->type->name), 0));
        } else{
            argTypes.push_back(TypeOf(*arg->type, context));
        }
    }
    Type* retType = nullptr;
    if( this->type->isArray )
        retType = PointerType::get(context.typeSystem.getVarType(this->type->name), 0);
    else
        retType = TypeOf(*this->type, context);

    FunctionType* functionType = FunctionType::get(retType, argTypes, false);
    Function* function = Function::Create(functionType, GlobalValue::ExternalLinkage, this->id->name.c_str(), context.theModule.get());

    if( !this->isExternal ){
        BasicBlock* basicBlock = BasicBlock::Create(context.llvmContext, "entry", function, nullptr);

        context.builder.SetInsertPoint(basicBlock);
        context.pushBlock(basicBlock);

        // declare function params
        auto origin_arg = this->arguments->begin();

        for(auto &ir_arg_it: function->args()){
            ir_arg_it.setName((*origin_arg)->id->name);
            //ir_arg_it.print(outs());
            Value* argAlloc;
            if( (*origin_arg)->type->isArray ){
                argAlloc = context.builder.CreateAlloca(PointerType::get(context.typeSystem.getVarType((*origin_arg)->type->name), 0));
            }else{
                argAlloc = (*origin_arg)->codeGen(context);
            }
            context.builder.CreateStore(&ir_arg_it, argAlloc, false);
            context.setSymbolValue((*origin_arg)->id->name, argAlloc);
            context.setSymbolType((*origin_arg)->id->name, (*origin_arg)->type->name);
            origin_arg++;
        }

        this->block->codeGen(context);
        if( context.getCurrentReturnValue() ){
            context.builder.CreateRet(context.getCurrentReturnValue());
        } else{
            return LogErrorV("Function block return value not founded");
        }
        context.popBlock();

    }


    return function;
}


llvm::Value* NStructDeclaration::codeGen(CodeGenContext& context) {
    cout << "Generating struct declaration of " << this->name->name << endl;

    std::vector<Type*> memberTypes;

//    context.builder.createstr
    auto structType = StructType::create(context.llvmContext, this->name->name);
    context.typeSystem.addStructType(this->name->name, structType);

    for(auto& member: *this->members){
        context.typeSystem.addStructMember(this->name->name, member->type->name, member->id->name);
        memberTypes.push_back(TypeOf(*member->type, context));
    }

    structType->setBody(memberTypes);

    return nullptr;
}

llvm::Value* NMethodCall::codeGen(CodeGenContext &context) {
    cout << "Generating method call of " << this->id->name << endl;
    Function * calleeF = context.theModule->getFunction(this->id->name);
    if( !calleeF ){
        LogErrorV("Function name not found");
    }
    if( calleeF->arg_size() != this->arguments->size() ){
        LogErrorV("Function arguments size not match");
    }
    std::vector<Value*> argsv;
    for(auto it=this->arguments->begin(); it!=this->arguments->end(); it++){
        argsv.push_back((*it)->codeGen(context));
        if( !argsv.back() ){        // if any argument codegen fail
            return nullptr;
        }
    }
    return context.builder.CreateCall(calleeF, argsv, "calltmp");
}

llvm::Value* NVariableDeclaration::codeGen(CodeGenContext &context) {
    cout << "Generating variable declaration of " << this->type->name << " " << this->id->name << endl;
    Type* type = TypeOf(*this->type, context);
    Value* initial = nullptr;

    Value* inst = nullptr;

    if( this->type->isArray ){
        auto arraySize = this->type->arraySize->codeGen(context);
        auto arrayType = ArrayType::get(context.typeSystem.getVarType(this->type->name), this->type->arraySize->value);
        inst = context.builder.CreateAlloca(arrayType, arraySize, "arraytmp");
//        inst = context.builder.CreatePointerCast(inst, PointerType::getUnqual(arrayType));
    }else{
        inst = context.builder.CreateAlloca(type);
    }

    context.setSymbolType(this->id->name, this->type->name);
    context.setSymbolValue(this->id->name, inst);

    context.PrintSymTable();

    if( this->assignmentExpr != nullptr ){
        NAssignment assignment(this->id, this->assignmentExpr);
        assignment.codeGen(context);
    }
    return inst;
}

llvm::Value* NReturnStatement::codeGen(CodeGenContext &context) {
    cout << "Generating return statement" << endl;
    Value* returnValue = this->expression->codeGen(context);
    context.setCurrentReturnValue(returnValue);
    return returnValue;
}

llvm::Value* NIfStatement::codeGen(CodeGenContext &context) {
    cout << "Generating if statement" << endl;
    Value* condValue = this->condition->codeGen(context);
    if( !condValue )
        return nullptr;

    condValue = CastToBoolean(context, condValue);

    Function* theFunction = context.builder.GetInsertBlock()->getParent();      // the function where if statement is in

    BasicBlock *thenBB = BasicBlock::Create(context.llvmContext, "then", theFunction);
    BasicBlock *falseBB = BasicBlock::Create(context.llvmContext, "else");
    BasicBlock *mergeBB = BasicBlock::Create(context.llvmContext, "ifcont");

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

    BasicBlock *block = BasicBlock::Create(context.llvmContext, "forloop", theFunction);
    BasicBlock *after = BasicBlock::Create(context.llvmContext, "forcont");

    // execute the initial
    if( this->initial )
        this->initial->codeGen(context);

    // fall to the block
    context.builder.CreateCondBr(condValue, block, after);

    context.builder.SetInsertPoint(block);

    context.pushBlock(block);

    this->block->codeGen(context);

    context.popBlock();

    // do increment
    if( this->increment ){
        this->increment->codeGen(context);
    }

    // execute the again or stop
    condValue = this->condition->codeGen(context);
    condValue = CastToBoolean(context, condValue);
    context.builder.CreateCondBr(condValue, block, after);

    // insert the after block
    theFunction->getBasicBlockList().push_back(after);
    context.builder.SetInsertPoint(after);

    return nullptr;
}

llvm::Value *NStructMember::codeGen(CodeGenContext &context) {
    cout << "Generating struct member expression of " << this->id->name << "." << this->member->name << endl;

    auto varPtr = context.getSymbolValue(this->id->name);
    auto structPtr = context.builder.CreateLoad(varPtr, "structPtr");
    structPtr->setAlignment(4);

    if( !structPtr->getType()->isStructTy() ){
        return LogErrorV("The variable is not struct");
    }

    string structName = structPtr->getType()->getStructName().str();
    long memberIndex = context.typeSystem.getStructMemberIndex(structName, this->member->name);

    std::vector<Value*> indices;
    indices.push_back(ConstantInt::get(context.typeSystem.intTy, 0, false));
    indices.push_back(ConstantInt::get(context.typeSystem.intTy, (uint64_t)memberIndex, false));
    auto ptr = context.builder.CreateInBoundsGEP(varPtr, indices, "memberPtr");

    return context.builder.CreateLoad(ptr);
}

llvm::Value* NStructAssignment::codeGen(CodeGenContext &context) {
    cout << "Generating struct assignment of " << this->structMember->id->name << "." << this->structMember->member->name << endl;
    auto varPtr = context.getSymbolValue(this->structMember->id->name);
    auto structPtr = context.builder.CreateLoad(varPtr, "structPtr");
//    auto underlyingStruct = context.builder.CreateLoad(load);
    structPtr->setAlignment(4);

    if( !structPtr->getType()->isStructTy() ){
        return LogErrorV("The variable is not struct");
    }

    string structName = structPtr->getType()->getStructName().str();
    long memberIndex = context.typeSystem.getStructMemberIndex(structName, this->structMember->member->name);

    std::vector<Value*> indices;
    auto value = this->expression->codeGen(context);
//    auto index = ;
    indices.push_back(ConstantInt::get(context.typeSystem.intTy, 0, false));
    indices.push_back(ConstantInt::get(context.typeSystem.intTy, (uint64_t)memberIndex, false));

    auto ptr = context.builder.CreateInBoundsGEP(varPtr, indices, "structMemberPtr");

    return context.builder.CreateStore(value, ptr);
}

llvm::Value *NArrayIndex::codeGen(CodeGenContext &context) {
    cout << "Generating array index expression of " << this->arrayName->name << endl;
    auto varPtr = context.getSymbolValue(this->arrayName->name);

//    auto arrayPtr = context.builder.CreateLoad(varPtr, "arrayPtr");
//    arrayPtr->setAlignment(16);

    if( !varPtr->getType()->isArrayTy() && !varPtr->getType()->isPointerTy() ){
        return LogErrorV("The variable is not array");
    }
//    std::vector<Value*> indices;
    auto value = this->expression->codeGen(context);
    ArrayRef<Value*> indices{ ConstantInt::get(Type::getInt64Ty(context.llvmContext), 0), value };
//    indices.push_back(0);
//    indices.push_back(value);
//    auto ptr = context.builder.CreateExtractElement(varPtr, value, "elementPtr");
    auto ptr = context.builder.CreateInBoundsGEP(varPtr, indices, "elementPtr");

//    ptr = context.builder.CreateBitCast(ptr, Type::getInt32PtrTy(context.llvmContext, 0), "castedPtr");
//    ptr = context.builder.CreatePointerCast(ptr, PointerType::get(context.typeSystem.getVarType(this->arrayName->name), 0), "castedPtr");
    return context.builder.CreateAlignedLoad(ptr, 4);
}


llvm::Value *NArrayAssignment::codeGen(CodeGenContext &context) {
    cout << "Generating array index assignment of " << this->arrayIndex->arrayName->name << endl;
    auto varPtr = context.getSymbolValue(this->arrayIndex->arrayName->name);

    if( varPtr == nullptr ){
        return LogErrorV("Unknown variable name");
    }
    
    auto arrayPtr = context.builder.CreateLoad(varPtr, "arrayPtr");
//    arrayPtr->setAlignment(16);

    if( !arrayPtr->getType()->isArrayTy() && !arrayPtr->getType()->isPointerTy() ){
        return LogErrorV("The variable is not array");
    }

//    std::vector<Value*> indices;
    auto index = this->arrayIndex->expression->codeGen(context);
    ArrayRef<Value*> gep2_array{ ConstantInt::get(Type::getInt64Ty(context.llvmContext), 0), index };
    auto ptr = context.builder.CreateInBoundsGEP(varPtr, gep2_array, "elementPtr");

    return context.builder.CreateAlignedStore(this->expression->codeGen(context), ptr, 4);
}

llvm::Value *NArrayInitialization::codeGen(CodeGenContext &context) {
    cout << "Generating array initialization of " << this->declaration->id->name << endl;
    auto arrayPtr = this->declaration->codeGen(context);
    for(int i=0; i < this->expressionList->size(); i++){
        shared_ptr<NInteger> index = make_shared<NInteger>(i);
        shared_ptr<NArrayIndex> arrayIndex = make_shared<NArrayIndex>(this->declaration->id, index);
        NArrayAssignment assignment(arrayIndex, this->expressionList->at(i));
        assignment.codeGen(context);
    }
    return nullptr;
}

llvm::Value *NLiteral::codeGen(CodeGenContext &context) {
    return context.builder.CreateGlobalString(this->value, "string");
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
