#include <iostream>
#include <fstream>
#include "ASTNodes.h"
#include "CodeGen.h"
#include "ObjGen.h"

extern shared_ptr<NBlock> programBlock;
extern int yyparse();
// extern void yyparse_init(const char* filename);
// extern void yyparse_cleanup();
//
//void createCoreFunctions(CodeGenContext& context);

int main(int argc, char **argv) {
    yyparse();

    // std::cout << programBlock << std::endl;
    programBlock->print("--");
    auto root = programBlock->jsonGen();

//    cout << root;

//    cout << root << endl;
    CodeGenContext context;
//    createCoreFunctions(context);
    context.generateCode(*programBlock);
    ObjGen(context);

    string jsonFile = "visualization/A_tree.json";
    std::ofstream astJson(jsonFile);
    if( astJson.is_open() ){
        astJson << root;
        astJson.close();
        cout << "json write to " << jsonFile << endl;
    }

    return 0;
}