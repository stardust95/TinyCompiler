//
// Created by cs on 2017/5/31.
//

#include "TypeSystem.h"
#include "CodeGen.h"
//
//Value* VarType::getDefaultValue(LLVMContext &context) const {
//    if( this->name == "int" ){
//        return ConstantInt::get(Type::getInt32Ty(context), 0, true);
//    }else if( this->name == "double" ){
//        return ConstantFP::get(Type::getDoubleTy(context), 0);
//    }
//    return nullptr;
//}
//
//Value* VarArrayType::getDefaultValue(LLVMContext &context) const {
//
//}

string TypeSystem::llvmTypeToStr(Type *value) {
    Type::TypeID typeID;
    if( value )
        typeID = value->getTypeID();
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


string TypeSystem::llvmTypeToStr(Value *value) {
    if( value )
        return llvmTypeToStr(value->getType());
    else
        return "Value is nullptr";

}

TypeSystem::TypeSystem(LLVMContext &context): llvmContext(context){
    addCast(intTy, floatTy, llvm::CastInst::SIToFP);
    addCast(intTy, doubleTy, llvm::CastInst::SIToFP);
    addCast(boolTy, doubleTy, llvm::CastInst::SIToFP);
    addCast(floatTy, doubleTy, llvm::CastInst::FPExt);
    addCast(floatTy, intTy, llvm::CastInst::FPToSI);
    addCast(doubleTy, intTy, llvm::CastInst::FPToSI);
    addCast(intTy, intTy, llvm::CastInst::SExt);
}

void TypeSystem::addStructMember(string structName, string memType, string memName) {
    if( this->_structTypes.find(structName) == this->_structTypes.end() ){
        LogError("Unknown struct name");
    }
    this->_structMembers[structName].push_back(std::make_pair(memType, memName));
}

void TypeSystem::addStructType(string name, llvm::StructType *type) {
    this->_structTypes[name] = type;
    this->_structMembers[name] = std::vector<TypeNamePair>();
}

Type *TypeSystem::getVarType(const NIdentifier& type) {
    assert(type.isType);
    if( type.isArray )
        return ArrayType::get(getVarType(type.name), type.arraySize->value);

    return getVarType(type.name);

    return 0;
}



Value* TypeSystem::getDefaultValue(string typeStr, LLVMContext &context) {
    Type* type = this->getVarType(typeStr);
    if( type == this->intTy ){
        return ConstantInt::get(type, 0, true);
    }else if( type == this->doubleTy || type == this->floatTy ){
        return ConstantFP::get(type, 0);
    }
    return nullptr;
}

void TypeSystem::addCast(Type *from, Type *to, CastInst::CastOps op) {
    if( _castTable.find(from) == _castTable.end() ){
        _castTable[from] = std::map<Type*, CastInst::CastOps>();
    }
    _castTable[from][to] = op;
}

Value* TypeSystem::cast(Value *value, Type *type, BasicBlock *block) {
    Type* from = value->getType();
    if( from == type )
        return value;
    if( _castTable.find(from) == _castTable.end() ){
        LogError("Type has no cast");
        return value;
    }
    if( _castTable[from].find(type) == _castTable[from].end() ){
        string error = "Unable to cast from ";
        error += llvmTypeToStr(from) + " to " + llvmTypeToStr(type);
        LogError(error.c_str());
        return value;
    }

    return CastInst::Create(_castTable[from][type], value, type, "cast", block);
}

bool TypeSystem::isStruct(string typeStr) const {
    return this->_structTypes.find(typeStr) != this->_structTypes.end();
}

long TypeSystem::getStructMemberIndex(string structName, string memberName) {
    if( this->_structTypes.find(structName) == this->_structTypes.end() ){
        LogError("Unknown struct name");
        return 0;
    }
    auto& members = this->_structMembers[structName];
    for(auto it=members.begin(); it!=members.end(); it++){
        if( it->second == memberName ){
            return std::distance(members.begin(), it);
        }
    }

    LogError("Unknown struct member");

    return 0;
}

Type *TypeSystem::getVarType(string typeStr) {

    if( typeStr.compare("int") == 0 ){
        return this->intTy;
    }
    if( typeStr.compare("float") == 0 ){
        return this->floatTy;
    }
    if( typeStr.compare("double") == 0 ){
        return this->doubleTy;
    }
    if( typeStr.compare("bool") == 0 ){
        return this->boolTy;
    }
    if( typeStr.compare("char") == 0 ){
        return this->charTy;
    }
    if( typeStr.compare("void") == 0 ){
        return this->voidTy;
    }
    if( typeStr.compare("string") == 0 ){
        return this->stringTy;
    }

    if( this->_structTypes.find(typeStr) != this->_structTypes.end() )
        return this->_structTypes[typeStr];

    return nullptr;
}

