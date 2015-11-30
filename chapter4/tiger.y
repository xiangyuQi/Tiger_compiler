%{
#include <stdio.h>
#include "util.h"
#include "symbol.h" 
#include "errormsg.h"
#include "absyn.h"


#define YYDEBUG 1
int yylex(void); /* function prototype */
A_exp absyn_root;
void yyerror(char *s)
{
 EM_error(EM_tokPos, "%s", s);
}
%}


%union {
	int pos;
	int ival;
	string sval;
	S_symbol sym;
	A_var var;
	A_exp exp;
	A_expList explist;
	A_efieldList efieldlist;
	A_dec dec;
	A_decList declist;
	A_namety namety;
	A_fundec fundec;
	A_ty ty;
	A_fieldList fieldlist;
}

%token <sval> ID STRING
%token <ival> INT

%right ASSIGN
%left AND OR
%nonassoc EQ NEQ LT LE GT GE
%left PLUS MINUS
%left TIMES DIVIDE
%right UMINUS

%token 
  COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK 
  LBRACE RBRACE DOT 
  PLUS MINUS TIMES DIVIDE EQ NEQ LT LE GT GE
  AND OR ASSIGN
  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF 
  BREAK NIL
  FUNCTION VAR TYPE 
  
%type <exp> exp program seq funcall record_cre array
%type <var>	lvalue
%type <explist> explist args
%type <efieldlist> efieldlist
%type <dec> dec vardec tydecs fundecs
%type <declist> declist
%type <namety> tydec
%type <fundec> fundec
%type <ty> ty
%type <fieldlist> fieldlist
%type <sym> id

/* et cetera */
  
%start program

%%

/* This is a skeleton grammar file, meant to illustrate what kind of
 * declarations are necessary above the %% mark.  Students are expected
 *  to replace the two dummy productions below with an actual grammar. 
 */

program : exp {absyn_root = $1;}

exp :   lvalue {$$ = A_VarExp(EM_tokPos,$1);}
	|  lvalue ASSIGN exp { $$ = A_AssignExp(EM_tokPos,$1,$3);}
	|  NIL	{$$ = A_NilExp(EM_tokPos);}
	|  seq  {$$ = $1;}
	|  INT  {$$ = A_IntExp(EM_tokPos,$1);}
	|  STRING {$$ = A_StringExp(EM_tokPos,$1);}
	|  MINUS exp %prec UMINUS {$$ = A_OpExp(EM_tokPos,A_minusOp,A_IntExp(EM_tokPos,0),$2);}
	|  funcall {$$ = $1;}
	|  exp PLUS exp {$$ = A_OpExp(EM_tokPos, A_plusOp, $1, $3);} 
	|  exp MINUS exp {$$ = A_OpExp(EM_tokPos, A_minusOp, $1, $3);} 
	|  exp TIMES exp {$$ = A_OpExp(EM_tokPos, A_timesOp, $1, $3);} 
	|  exp DIVIDE exp {$$ = A_OpExp(EM_tokPos, A_divideOp, $1, $3);} 
	|  exp EQ exp {$$ = A_OpExp(EM_tokPos, A_eqOp, $1, $3);} 
	|  exp NEQ exp {$$ = A_OpExp(EM_tokPos, A_neqOp, $1, $3);} 
	|  exp LT exp {$$ = A_OpExp(EM_tokPos, A_ltOp, $1, $3);} 
	|  exp LE exp {$$ = A_OpExp(EM_tokPos, A_leOp, $1, $3);} 
	|  exp GT exp {$$ = A_OpExp(EM_tokPos, A_gtOp, $1, $3);} 
	|  exp GE exp {$$ = A_OpExp(EM_tokPos, A_geOp, $1, $3);} 
	|  exp AND exp {$$ = A_IfExp(EM_tokPos,$1,$3,A_IntExp(EM_tokPos,0));}
	|  exp OR exp  {$$ = A_IfExp(EM_tokPos,$1,A_IntExp(EM_tokPos,1),$3);}
	|  record_cre {$$ = $1;}
	|  array	  {$$ = $1;}	
	|  IF exp THEN exp ELSE exp {$$ = A_IfExp(EM_tokPos,$2,$4,$6);}
    |  IF exp THEN exp	{$$ = A_IfExp(EM_tokPos,$2,$4,NULL);}
	|  WHILE exp DO exp {$$ = A_WhileExp(EM_tokPos,$2,$4);}
	|  FOR id ASSIGN exp TO exp DO exp {$$ = A_ForExp(EM_tokPos,$2,$4,$6,$8);}
	|  BREAK  {$$ = A_BreakExp(EM_tokPos);} 
	|  LET declist IN explist END  { $$ = A_LetExp(EM_tokPos,$2,A_SeqExp(EM_tokPos,$4));}

seq : LPAREN explist RPAREN {$$ = A_SeqExp(EM_tokPos,$2);}

explist: exp SEMICOLON explist {$$ = A_ExpList($1,$3);}
	   | exp {$$ = A_ExpList($1,NULL);}
	   | {$$ = NULL;}
	   

funcall : id LPAREN args RPAREN	{$$ = A_CallExp(EM_tokPos,$1,$3);}

args:  exp COMMA args {$$ = A_ExpList($1,$3);}
	|  exp {$$= A_ExpList($1,NULL); }
	|  {$$ =NULL;}
	
record_cre: id LBRACE efieldlist RBRACE {$$ = A_RecordExp(EM_tokPos,$1,$3);}
	
efieldlist : id EQ exp COMMA efieldlist {$$ = A_EfieldList(A_Efield($1,$3),$5);}
			  | id EQ exp  {$$ = A_EfieldList(A_Efield($1,$3),NULL);}
			  | {$$ =NULL;}
			  

declist : dec declist {$$ = A_DecList($1,$2);}
	 |	{$$ = NULL;}
	
dec : tydecs {$$ = $1;} 
	| vardec   {$$ = $1;}
	| fundecs {$$ = $1;}

tydecs  :tydec tydecs {$$ = A_TypeDec(EM_tokPos,A_NametyList($1,$2->u.type));}
		| tydec	{$$ = A_TypeDec(EM_tokPos,A_NametyList($1,NULL));}
			
tydec : TYPE id EQ ty {$$ = A_Namety($2,$4);}
	
ty : id			{$$ = A_NameTy(EM_tokPos,$1);}
   | LBRACE fieldlist RBRACE {$$ = A_RecordTy(EM_tokPos,$2);}
   | ARRAY OF id {$$ = A_ArrayTy(EM_tokPos,$3);}


fieldlist : id COLON id COMMA fieldlist {$$ = A_FieldList(A_Field(EM_tokPos,$1,$3),$5);}
		 | id COLON id {$$ = A_FieldList(A_Field(EM_tokPos,$1,$3),NULL);}
		 | {$$ =NULL;} 
vardec : VAR id ASSIGN exp  {$$ = A_VarDec(EM_tokPos,$2,NULL,$4);}
	   | VAR id COLON id ASSIGN exp {$$ = A_VarDec(EM_tokPos,$2,$4,$6);}
	   
fundecs : fundec fundecs {$$ =A_FunctionDec(EM_tokPos,A_FundecList($1,$2->u.function));}
	   |  fundec  {$$ = $$ =A_FunctionDec(EM_tokPos,A_FundecList($1,NULL));}
	   
fundec : FUNCTION id LPAREN fieldlist RPAREN EQ exp {$$ = A_Fundec(EM_tokPos,$2,$4,NULL,$7);}
	   | FUNCTION id LPAREN fieldlist RPAREN COLON id EQ exp {$$ = A_Fundec(EM_tokPos,$2,$4,$7,$9);}
	
lvalue : id   {$$ = A_SimpleVar(EM_tokPos,$1);}
	   | lvalue DOT id {$$ = A_FieldVar(EM_tokPos,$1,$3);}
	   | lvalue LBRACK exp RBRACK {$$ = A_SubscriptVar(EM_tokPos,$1,$3);}
	   | id LBRACK exp RBRACK {$$ = A_SubscriptVar(EM_tokPos, A_SimpleVar(EM_tokPos, $1), $3);}
array : id LBRACK exp RBRACK OF exp {$$ = A_ArrayExp(EM_tokPos,$1,$3,$6);}
id: ID {$$ = S_Symbol($1);}