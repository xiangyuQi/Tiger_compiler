%{
#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#define YYDEBUG 1
int yylex(void); /* function prototype */

void yyerror(char *s)
{
 EM_error(EM_tokPos, "%s", s);
}
%}


%union {
	int pos;
	int ival;
	string sval;
	}

%token <sval> ID STRING
%token <ival> INT

%left ASSIGN
%left AND OR
%nonassoc EQ NEQ LT LE GT GE
%left PLUS MINUS
%left TIMES DIVIDE
%right DO OF ELSE
%right UMINUS

%token 
  COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK 
  LBRACE RBRACE DOT 
  PLUS MINUS TIMES DIVIDE EQ NEQ LT LE GT GE
  AND OR ASSIGN
  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF 
  BREAK NIL
  FUNCTION VAR TYPE 

%start program

%%

/* This is a skeleton grammar file, meant to illustrate what kind of
 * declarations are necessary above the %% mark.  Students are expected
 *  to replace the two dummy productions below with an actual grammar. 
 */

program : exp

exp :  lvalue
	|  lvalue ASSIGN exp
	|  NIL
	|  seq
	|  INT
	|  STRING
	|  MINUS exp %prec UMINUS
	|  funcal
	|  exp PLUS exp
	|  exp MINUS exp
	|  exp TIMES exp
	|  exp DIVIDE exp
	|  exp EQ exp
	|  exp NEQ exp
	|  exp LT exp
	|  exp LE exp
	|  exp GT exp
	|  exp GE exp
	|  exp AND exp
	|  exp OR exp
	|  record_cre
	|  array
	|  IF exp THEN exp ELSE exp
	|  IF exp THEN exp

	|  WHILE exp DO exp
	|  FOR ID ASSIGN exp TO exp DO exp
	|  BREAK 
	|  LET decs IN explist END 

seq : LPAREN explist RPAREN
	
explist: exp SEMICOLON explist
	   | exp
	

funcal : ID LPAREN args RPAREN	

args:  exp COMMA args
	|  exp
	|
	
record_cre: ID LBRACE recode_fields RBRACE
	
recode_fields : ID EQ exp COMMA recode_fields
			  | ID EQ exp 
			  |

decs : dec decs
	 |
	
dec : tydec
	| vardec
	| fundec
	
tydec : TYPE ID EQ ty
	
ty : ID
   | LBRACE tyfields RBRACE
   | ARRAY OF ID

/*tyfields : tyfield tail_tyfields
		   |
	
tail_tyfields : COMMA tyfield tail_tyfields
			  |

tyfield : ID COLON ID
*/
tyfields : ID COLON ID COMMA tyfields
		 | ID COLON ID 
	     |
vardec : VAR ID ASSIGN exp
	   | VAR ID COLON ID ASSIGN exp
		
fundec : FUNCTION ID LPAREN tyfields RPAREN EQ exp
	   | FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp
	
lvalue : ID 
	   | lvalue DOT ID 
	   | lvalue LBRACK exp RBRACK
	
array : ID LBRACK exp RBRACK OF exp