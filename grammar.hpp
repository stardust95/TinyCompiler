/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_GRAMMAR_HPP_INCLUDED
# define YY_YY_GRAMMAR_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TIDENTIFIER = 258,
    TINTEGER = 259,
    TDOUBLE = 260,
    TCEQ = 261,
    TCNE = 262,
    TCLT = 263,
    TCLE = 264,
    TCGT = 265,
    TCGE = 266,
    TEQUAL = 267,
    TLPAREN = 268,
    TRPAREN = 269,
    TLBRACE = 270,
    TRBRACE = 271,
    TCOMMA = 272,
    TDOT = 273,
    TPLUS = 274,
    TMINUS = 275,
    TMUL = 276,
    TDIV = 277,
    TIF = 278,
    TELSE = 279,
    TFOR = 280,
    TWHILE = 281
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 12 "grammar.y" /* yacc.c:1915  */

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

#line 94 "grammar.hpp" /* yacc.c:1915  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_GRAMMAR_HPP_INCLUDED  */
