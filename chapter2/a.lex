%{
#include "tokens.h"
#include "errormsg.h"
#include "util.h"
#include <string.h>
union{
  int ival;
  string sval;
  double fval;
}yylval;
#define ADJ{EM_tokPos = charPos,charPos += yyleng}
%}
digits [0-9]+
%%
if                     {ADJ;return IF;}
[a-z][a-z0-9]*         {ADJ; yylval.sval= String(yytext);return ID;}
{digits}        	   {ADJ; yylval.ival = atoi(yytext);return NUM;}
({digits}"."[0-9]*)|([0-9]*"."{digits}) 
                       {ADJ;yylval.fval=atof(yytext);return REAL;}
("-"[a-z]*"\n")|(" "|"\n"|"\t")+  {ADJ;}
                    {ADJ:EM_error("illegal character");}
%%
int yywrap()
{
  return 1;
}