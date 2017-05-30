#ifndef __CODEGEN_H__
#define __CODEGEN_H__


#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <stack>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include "ASTNodes.h"
#include "grammar.hpp"

using namespace llvm;
using std::unique_ptr;
using std::string;

using SymTable = std::map<string, Value*>;

class CodeGenBlock{
public:
    BasicBlock * block;
    Value * returnValue;
    std::map<string, Value*> locals;
    std::map<string, string> types;     // type name string of vars
};

class CodeGenContext{
private:
    std::vector<CodeGenBlock*> blockStack;

public:
    LLVMContext llvmContext;
    IRBuilder<> builder;
    unique_ptr<Module> theModule;
    SymTable globalVars;

    CodeGenContext(): builder(llvmContext){
        theModule = unique_ptr<Module>(new Module("main", getGlobalContext()));
    }

//    SymTable& locals() const{
//        return blockStack.top()->locals;
//    };

//    std::map<string, string>& types() const{
//        return blockStack.back()->types;
//    };

    Value* getSymbolValue(string name) const{
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
//            cout << "(*it)->locals[" << name << "] = " << (*it)->locals[name] << endl;
            if( (*it)->locals.find(name) != (*it)->locals.end() ){
                return (*it)->locals[name];
            }
        }
        return nullptr;
    }

    string getSymbolType(string name) const{
        for(auto it=blockStack.rbegin(); it!=blockStack.rend(); it++){
//            cout << "(*it)->locals[" << name << "] = " << (*it)->locals[name] << endl;
            if( (*it)->types.find(name) != (*it)->types.end() ){
                return (*it)->types[name];
            }
        }
        return "";
    }

    void setSymbolValue(string name, Value* value){
        blockStack.back()->locals[name] = value;
    }

    void setSymbolType(string name, string value){
        blockStack.back()->types[name] = value;
    }


    BasicBlock* currentBlock() const{
        return blockStack.back()->block;
    }

    void pushBlock(BasicBlock * block){
        CodeGenBlock * codeGenBlock = new CodeGenBlock();
        codeGenBlock->block = block;
        codeGenBlock->returnValue = nullptr;
        blockStack.push_back(codeGenBlock);
    }

    void popBlock(){
        CodeGenBlock * codeGenBlock = blockStack.back();
        blockStack.pop_back();
        delete codeGenBlock;
    }

    void setCurrentReturnValue(Value* value){
        blockStack.back()->returnValue = value;
    }

    Value* getCurrentReturnValue(){
        return blockStack.back()->returnValue;
    }

    void PrintSymTable() const{
        cout << "======= Print Symbol Table ========" << endl;
        string prefix = "";
        for(auto it=blockStack.begin(); it!=blockStack.end(); it++){
            for(auto it2=(*it)->locals.begin(); it2!=(*it)->locals.end(); it2++){
                cout << prefix << it2->first << " = " << it2->second << ": " << this->getSymbolType(it2->first) << endl;
            }
            prefix += "\t";
        }
        cout << "===================================" << endl;
    }

    void generateCode(NBlock& );

    void runCode();

};

Value* LogErrorV(const char* str);
Value* LogErrorV(string str);

#endif