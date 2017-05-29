#include <iostream>
#include "ASTNodes.h"

extern NBlock* programBlock;
extern int yyparse();
// extern void yyparse_init(const char* filename);
// extern void yyparse_cleanup();

int main(int argc, char **argv) {
    yyparse();

    // std::cout << programBlock << std::endl;
    programBlock->print("--");
    return 0;
}