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
    TYINT = 261,
    TYDOUBLE = 262,
    TYFLOAT = 263,
    TYCHAR = 264,
    TYBOOL = 265,
    TYVOID = 266,
    TYSTRING = 267,
    TEXTERN = 268,
    TLITERAL = 269,
    TCEQ = 270,
    TCNE = 271,
    TCLT = 272,
    TCLE = 273,
    TCGT = 274,
    TCGE = 275,
    TEQUAL = 276,
    TLPAREN = 277,
    TRPAREN = 278,
    TLBRACE = 279,
    TRBRACE = 280,
    TCOMMA = 281,
    TDOT = 282,
    TSEMICOLON = 283,
    TLBRACKET = 284,
    TRBRACKET = 285,
    TQUOTATION = 286,
    TPLUS = 287,
    TMINUS = 288,
    TMUL = 289,
    TDIV = 290,
    TAND = 291,
    TOR = 292,
    TXOR = 293,
    TMOD = 294,
    TNEG = 295,
    TNOT = 296,
    TSHIFTL = 297,
    TSHIFTR = 298,
    TIF = 299,
    TELSE = 300,
    TFOR = 301,
    TWHILE = 302,
    TRETURN = 303,
    TSTRUCT = 304
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 12 "grammar.y" /* yacc.c:1915  */

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

#line 117 "grammar.hpp" /* yacc.c:1915  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_GRAMMAR_HPP_INCLUDED  */
