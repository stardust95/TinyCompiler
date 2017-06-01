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
	NBlock* block;
	NExpression* expr;
	NStatement* stmt;
	NIdentifier* ident;
	NVariableDeclaration* var_decl;
	NArrayIndex* index;
	std::vector<shared_ptr<NVariableDeclaration>>* varvec;
	std::vector<shared_ptr<NExpression>>* exprvec;
	std::string* string;
	int token;
}

%token <string> TIDENTIFIER TINTEGER TDOUBLE TYINT TYDOUBLE TYFLOAT TYCHAR TYBOOL TYVOID
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT TSEMICOLON TLBRACKET TRBRACKET
%token <token> TPLUS TMINUS TMUL TDIV TAND TOR TXOR TMOD TNEG TNOT TSHIFTL TSHIFTR
%token <token> TIF TELSE TFOR TWHILE TRETURN TSTRUCT

%type <index> array_index
%type <ident> ident typename primary_typename
%type <expr> numeric expr assign
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
stmts : stmt { $$ = new NBlock(); $$->statements->push_back(shared_ptr<NStatement>($1)); }
			| stmts stmt { $1->statements->push_back(shared_ptr<NStatement>($2)); }
			;
stmt : var_decl | func_decl | struct_decl
		 | expr { $$ = new NExpressionStatement(shared_ptr<NExpression>($1)); }
		 | TRETURN expr { $$ = new NReturnStatement(shared_ptr<NExpression>($2)); }
		 | if_stmt
		 | for_stmt
		 | while_stmt
		 ;

block : TLBRACE stmts TRBRACE { $$ = $2; }
			| TLBRACE TRBRACE { $$ = new NBlock(); }
			;

primary_typename : TYINT { $$ = new NIdentifier(*$1); delete $1; }
					| TYDOUBLE { $$ = new NIdentifier(*$1); delete $1; }
					| TYFLOAT { $$ = new NIdentifier(*$1); delete $1; }
					| TYCHAR { $$ = new NIdentifier(*$1); delete $1; }
					| TYBOOL { $$ = new NIdentifier(*$1); delete $1; }
					| TYVOID { $$ = new NIdentifier(*$1); delete $1; }

typename : primary_typename { $1->isType = true; $$ = $1; }
			| primary_typename TLBRACKET TINTEGER TRBRACKET { 
				$1->isType = true; $1->isArray = true; 
				$1->arraySize = make_shared<NInteger>(atol($3->c_str())); 
				$$ = $1; 
			}
			| TSTRUCT ident {
				$2->isType = true;
				$$ = $2;
			}

var_decl : typename ident { $$ = new NVariableDeclaration(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($2), nullptr); }
				 | typename ident TEQUAL expr { $$ = new NVariableDeclaration(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($2), shared_ptr<NExpression>($4)); }
				 | typename ident TEQUAL TLBRACKET call_args TRBRACKET {
					 $$ = new NArrayInitialization(make_shared<NVariableDeclaration>(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($2), nullptr), shared_ptr<ExpressionList>($5));
				 }
				 ;

func_decl : typename ident TLPAREN func_decl_args TRPAREN block
					{ $$ = new NFunctionDeclaration(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($2), shared_ptr<VariableList>($4), shared_ptr<NBlock>($6));  }
					;

func_decl_args : /* blank */ { $$ = new VariableList(); }
							 | var_decl { $$ = new VariableList(); $$->push_back(shared_ptr<NVariableDeclaration>($<var_decl>1)); }
							 | func_decl_args TCOMMA var_decl { $1->push_back(shared_ptr<NVariableDeclaration>($<var_decl>3)); }
							 ;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
			;

numeric : TINTEGER { $$ = new NInteger(atol($1->c_str())); }
				| TDOUBLE { $$ = new NDouble(atof($1->c_str())); }
				;
expr : 	assign { $$ = $1; }
		 | ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(shared_ptr<NIdentifier>($1), shared_ptr<ExpressionList>($3)); }
		 | ident { $<ident>$ = $1; }
		 | ident TDOT ident { $$ = new NStructMember(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($3)); }
		 | numeric
		 | expr comparison expr { $$ = new NBinaryOperator(shared_ptr<NExpression>($1), $2, shared_ptr<NExpression>($3)); }
		 | TLPAREN expr TRPAREN { $$ = $2; }
		 | TMINUS expr { $$ = nullptr; /* TODO */ }
		 | array_index { $$ = $1; }
		 ;

array_index : ident TLBRACKET expr TRBRACKET { $$ = new NArrayIndex(shared_ptr<NIdentifier>($1), shared_ptr<NExpression>($3)); }

assign : ident TEQUAL expr { $$ = new NAssignment(shared_ptr<NIdentifier>($1), shared_ptr<NExpression>($3)); }
			| array_index TEQUAL expr {
				$$ = new NArrayAssignment(shared_ptr<NArrayIndex>($1), shared_ptr<NExpression>($3));
			}
			| ident TDOT ident TEQUAL expr {
				auto member = make_shared<NStructMember>(shared_ptr<NIdentifier>($1), shared_ptr<NIdentifier>($3)); 
				$$ = new NStructAssignment(member, shared_ptr<NExpression>($5)); 
			}
			;

call_args : /* blank */ { $$ = new ExpressionList(); }
					| expr { $$ = new ExpressionList(); $$->push_back(shared_ptr<NExpression>($1)); }
					| call_args TCOMMA expr { $1->push_back(shared_ptr<NExpression>($3)); }
comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE
					 | TPLUS | TMINUS | TMUL | TDIV | TAND | TMOD | TOR | TXOR | TSHIFTL | TSHIFTR
					 ;
if_stmt : TIF expr block { $$ = new NIfStatement(shared_ptr<NExpression>($2), shared_ptr<NBlock>($3)); }
		| TIF expr block TELSE block { $$ = new NIfStatement(shared_ptr<NExpression>($2), shared_ptr<NBlock>($3), shared_ptr<NBlock>($5)); }
		| TIF expr block TELSE if_stmt { 
			auto blk = new NBlock(); 
			blk->statements->push_back(shared_ptr<NStatement>($5)); 
			$$ = new NIfStatement(shared_ptr<NExpression>($2), shared_ptr<NBlock>($3), shared_ptr<NBlock>(blk)); 
		}

for_stmt : TFOR TLPAREN expr TSEMICOLON expr TSEMICOLON expr TRPAREN block { $$ = new NForStatement(shared_ptr<NBlock>($9), shared_ptr<NExpression>($3), shared_ptr<NExpression>($5), shared_ptr<NExpression>($7)); }
		
while_stmt : TWHILE TLPAREN expr TRPAREN block { $$ = new NForStatement(shared_ptr<NBlock>($5), nullptr, shared_ptr<NExpression>($3), nullptr); }

struct_decl : TSTRUCT ident TLBRACE struct_members TRBRACE {$$ = new NStructDeclaration(shared_ptr<NIdentifier>($2), shared_ptr<VariableList>($4)); }

struct_members : /* blank */ { $$ = new VariableList(); }
				| var_decl { $$ = new VariableList(); $$->push_back(shared_ptr<NVariableDeclaration>($<var_decl>1)); }
				| struct_members var_decl { $1->push_back(shared_ptr<NVariableDeclaration>($<var_decl>2)); }

%%