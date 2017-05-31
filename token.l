%{
#include <stdio.h>
#include <string>
#include "ASTNodes.h"
#include "grammar.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)

static FILE* yyparse_file_ptr;

// void yyparse_init(const char* filename)
// {
// 	yyparse_file_ptr = fopen(filename, "r");
// 	yyin = yyparse_file_ptr;
// }

// void yyparse_cleanup()
// {
// 	fclose(yyparse_file_ptr);
// }

%}

%option noyywrap

%%

[ \t\r\n]				;
"if"                    return TOKEN(TIF);
"else"                  return TOKEN(TELSE);
"return"                 return TOKEN(TRETURN);
"for"                   return TOKEN(TFOR);
"while"                 return TOKEN(TWHILE);
"struct"                return TOKEN(TSTRUCT);
[a-zA-Z_][a-zA-Z0-9_]*	SAVE_TOKEN; return TIDENTIFIER;
[0-9]+\.[0-9]*			SAVE_TOKEN; return TDOUBLE;
[0-9]+  				SAVE_TOKEN; return TINTEGER;
"="						return TOKEN(TEQUAL);
"=="					return TOKEN(TCEQ);
"!="                    return TOKEN(TCNE);
"<"                     return TOKEN(TCLT);
"<="                    return TOKEN(TCLE);
">"                     return TOKEN(TCGT);
">="                    return TOKEN(TCGE);
"("                     return TOKEN(TLPAREN);
")"                     return TOKEN(TRPAREN);
"{"                     return TOKEN(TLBRACE);
"}"                     return TOKEN(TRBRACE);
"["                     return TOKEN(TLBRACKET);
"]"                     return TOKEN(TRBRACKET);
"."                     return TOKEN(TDOT);
","                     return TOKEN(TCOMMA);
"+"                     return TOKEN(TPLUS);
"-"                     return TOKEN(TMINUS);
"*"                     return TOKEN(TMUL);
"/"                     return TOKEN(TDIV);
"&"                     return TOKEN(TAND);
"|"                     return TOKEN(TOR);
"^"                     return TOKEN(TXOR);
"%"                     return TOKEN(TMOD);
">>"                    return TOKEN(TSHIFTR);
"<<"                    return TOKEN(TSHIFTL);
";"                     return TOKEN(TSEMICOLON);
.						printf("Unknown token:%s\n", yytext); yyterminate();



%%