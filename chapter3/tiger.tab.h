
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     STRING = 259,
     INT = 260,
     ASSIGN = 261,
     OR = 262,
     AND = 263,
     GE = 264,
     GT = 265,
     LE = 266,
     LT = 267,
     NEQ = 268,
     EQ = 269,
     MINUS = 270,
     PLUS = 271,
     DIVIDE = 272,
     TIMES = 273,
     ELSE = 274,
     OF = 275,
     DO = 276,
     UMINUS = 277,
     COMMA = 278,
     COLON = 279,
     SEMICOLON = 280,
     LPAREN = 281,
     RPAREN = 282,
     LBRACK = 283,
     RBRACK = 284,
     LBRACE = 285,
     RBRACE = 286,
     DOT = 287,
     ARRAY = 288,
     IF = 289,
     THEN = 290,
     WHILE = 291,
     FOR = 292,
     TO = 293,
     LET = 294,
     IN = 295,
     END = 296,
     BREAK = 297,
     NIL = 298,
     FUNCTION = 299,
     VAR = 300,
     TYPE = 301
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 15 "tiger.y"

	int pos;
	int ival;
	string sval;
	


/* Line 1676 of yacc.c  */
#line 106 "tiger.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


