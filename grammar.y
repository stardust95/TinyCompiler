%{
	#include "ASTNodes.h"
	#include <stdio.h>
	NBlock* programBlock;
	extern int yylex();
	void yyerror(const char* s)
	{
		printf("Error: %s\n", s);
	}
%}
%union
{
	Node* node;
	NBlock* block;
	NExpression* expr;
	NStatement* stmt;
	NIdentifier* ident;
	NVariableDeclaration* var_decl;
	std::vector<NVariableDeclaration*>* varvec;
	std::vector<NExpression*>* exprvec;
	std::string* string;
	int token;
}
%token <string> TIDENTIFIER TINTEGER TDOUBLE
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT TSEMICOLON TLBRACKET TRBRACKET
%token <token> TPLUS TMINUS TMUL TDIV TAND TOR TXOR TMOD TNEG TNOT TSHIFTL TSHIFTR
%token <token> TIF TELSE TFOR TWHILE TRETURN TSTRUCT

%type <ident> ident
%type <expr> numeric expr assign struct_member
%type <varvec> func_decl_args struct_members
%type <exprvec> call_args
%type <block> program stmts block
%type <stmt> stmt var_decl func_decl struct_decl if_stmt for_stmt while_stmt
%type <token> comparison

%left TPLUS TMINUS
%left TMUL TDIV
%start program

%%
program : stmts { programBlock = $1; }
				;
stmts : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
			| stmts stmt { $1->statements.push_back($<stmt>2); }
			;
stmt : var_decl | func_decl | struct_decl
		 | expr { $$ = new NExpressionStatement(*$1); }
		 | TRETURN expr { $$ = new NReturnStatement(*$2); }
		 | if_stmt
		 | for_stmt
		 | while_stmt
		 ;
block : TLBRACE stmts TRBRACE { $$ = $2; }
			| TLBRACE TRBRACE { $$ = new NBlock(); }
			;

var_decl : ident ident { $$ = new NVariableDeclaration(*$1, *$2, nullptr); }
				 | ident ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
				 ;
func_decl : ident ident TLPAREN func_decl_args TRPAREN block
					{ $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
					;


func_decl_args : /* blank */ { $$ = new VariableList(); }
							 | var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
							 | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
							 ;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
			;

numeric : TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
				| TDOUBLE { $$ = new NDouble(atof($1->c_str())); delete $1; }
				;
expr : 	assign { $$ = $1; }
		 | ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3; }
		 | ident { $<ident>$ = $1; }
		 | ident TDOT ident { $$ = new NStructMember(*$1, *$3); }
		 | numeric
		 | expr comparison expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
		 | TLPAREN expr TRPAREN { $$ = $2; }
		 | TMINUS expr { $$ = nullptr; /* TODO */ }
		 | { $$ = nullptr; }
		 ;

assign : ident TEQUAL expr { $$ = new NAssignment(*$<ident>1, *$3); }
			| ident TDOT ident TEQUAL expr { auto member = new NStructMember(*$1, *$3); 
			$$ = new NStructAssignment(*member, *$5); }

call_args : /* blank */ { $$ = new ExpressionList(); }
					| expr { $$ = new ExpressionList(); $$->push_back($1); }
					| call_args TCOMMA expr { $1->push_back($3); }
comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
					 | TPLUS | TMINUS | TMUL | TDIV | TAND | TMOD | TOR | TXOR | TSHIFTL | TSHIFTR
					 ;
if_stmt : TIF expr block { $$ = new NIfStatement(*$2, $3); }
		| TIF expr block TELSE block { $$ = new NIfStatement(*$2, $3, $5); }

for_stmt : TFOR TLPAREN expr TSEMICOLON expr TSEMICOLON expr TRPAREN block { $$ = new NForStatement(*$9, $3, $5, $7); }
		
while_stmt : TWHILE TLPAREN expr TRPAREN block { $$ = new NForStatement(*$5, nullptr, $3, nullptr); }

struct_decl : TSTRUCT ident TLBRACE struct_members TRBRACE { $$ = new NStructDeclaration(*$2, *$4); delete $4; }

struct_members : /* blank */ { $$ = new VariableList(); }
				| var_decl { $$ = new VariableList(); $$->push_back($<var_decl>1); }
				| struct_members var_decl { $1->push_back($<var_decl>2); }

%%