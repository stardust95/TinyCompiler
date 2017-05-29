#include <iostream>
#include "ASTNodes.h"
#include "CodeGen.h"

extern NBlock* programBlock;
extern int yyparse();
// extern void yyparse_init(const char* filename);
// extern void yyparse_cleanup();

void createCoreFunctions(CodeGenContext& context);

int main(int argc, char **argv) {
    yyparse();

    // std::cout << programBlock << std::endl;
    programBlock->print("--");

    CodeGenContext context;
//    createCoreFunctions(context);
    context.generateCode(*programBlock);


    return 0;
}