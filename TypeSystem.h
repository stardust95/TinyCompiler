//
// Created by cs on 2017/5/31.
//

#ifndef TINYCOMPILER_TYPESYSTEM_H
#define TINYCOMPILER_TYPESYSTEM_H

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <string>
#include <map>
#include <vector>

#include "ASTNodes.h"

using std::string;
using namespace llvm;

//
//struct VarType {
//    string name;
//
//    VarType(string name): name(name){}
//
//    virtual Value* getDefaultValue(LLVMContext & context) const;
//
//    virtual bool isArray() const{
//        return false;
//    }
//
//    virtual bool isStruct() const{
//        return false;
//    }
//
//};
//
//struct VarArrayType: VarType{
//
//    NExpression & size;
//
//    VarArrayType(string name, NExpression& size)
//            :VarType(name), size(size){
//
//    }
//
//    Value *getDefaultValue(LLVMContext &context) const override;
//
//
//    bool isArray() const override{
//        return true;
//    }
//
//};
//
//struct VarStructType: VarType{
//    VarStructType(string name): VarType(name){}
//
//    Value *getDefaultValue(LLVMContext &context) const override ;
//
//    bool isStruct() const override{
//        return true;
//    }
//};

using TypeNamePair = std::pair<std::string, std::string>;

class TypeSystem{
private:

    LLVMContext& llvmContext;

    std::map<string, std::vector<TypeNamePair>> _structMembers;

    std::map<string, llvm::StructType*> _structTypes;

    std::map<Type*, std::map<Type*, CastInst::CastOps>> _castTable;

    void addCast(Type* from, Type* to, CastInst::CastOps op);

public:
    Type* floatTy = Type::getFloatTy(llvmContext);
    Type* intTy = Type::getInt32Ty(llvmContext);
    Type* charTy = Type::getInt8Ty(llvmContext);
    Type* doubleTy = Type::getDoubleTy(llvmContext);
    Type* stringTy = Type::getInt8PtrTy(llvmContext);
    Type* voidTy = Type::getVoidTy(llvmContext);
    Type* boolTy = Type::getInt1Ty(llvmContext);


    TypeSystem(LLVMContext& context);

    void addStructType(string structName, llvm::StructType*);

    void addStructMember(string structName, string memType, string memName);

    Type* getVarType(string typeStr) const;

    Value* getDefaultValue(string typeStr, LLVMContext &context) const;

    Value* cast(Value* value, Type* type, BasicBlock* block);

    static string llvmTypeToStr(Value* value) ;
    static string llvmTypeToStr(Type* type) ;
};


#endif //TINYCOMPILER_TYPESYSTEM_H
