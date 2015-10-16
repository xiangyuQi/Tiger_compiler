%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

#difine MAX_LENGTH 512

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}

void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}


/* user-def */
static char * str_ptr;
static char str[MAX_LENGTH]; /* save string like "..." */
static char ch;              /* save trans-meaning char */
static int remain;
static int comment_nest = 0;

void init_string()
{
   remain = MAXLENGTH - 1;
   str_ptr = str;
}

#define OVER_MEM_ERR printf("%s (max_length: %d)","usage: string out of memrory!",MAX_LENGHT);\
            exit(1);
			
void append_char_to_string(int ch)
{
  if(!remain)
  {
     OVER_MEM_ERR;
  }
  *str_ptr++ = ch;
  remain--;
}	

void append_str_to_string(char *s)
{
   int t = strlen(s);
   if(remain <t)
   {
      OVER_MEM_ERR;
   }
   while((*str_ptr++ = *s++,*s));//拷贝s到string
   remain -= t;
}
void end_string()
{
   if(!remain)
   {
      OVER_MEM_ERR；
   }
   *str_ptr++ ='\0';
}	


%}

id [A-Za-z][_A-Za-z0-9]*
digits [0-9]+
	
%%
" "	 {adjust(); continue;}
\n	 {adjust(); EM_newline(); continue;}
","	 {adjust(); return COMMA;}
":=" {adjust(); return ASSIGN;}
":"  {adjust(); return COLON;}
";"  {adjust(); return SEMICOLON;}
"("  {adjust(); return LPAREN;}
")"  {adjust(); return RPAREN;}
"["  {adjust(); return LBRACE;}
"]"  {adjust(); return RBRACE;}
"."  {adjust(); return DOT;}
"+"  {adjust(); return PLUS;}
"-"  {adjust(); return MINUS;}
"*"  {adjust(); return TIMES;}
"/"  {adjust(); return DIVIDE;} 
"="  {adjust(); return EQ;}
"<>" {adjust(); return NEQ;}
"<=" {adjust(); return LE;}
"<"  {adjust(); return LT;}
">=" {adjust(); return GE;}
">"  {adjust(); return GT;}  
"&"  {adjust(); return AND;}
"|"  {adjust(); return OR;}
while    {adjust(); return WHILE;} 
for  	 {adjust(); return FOR;}
to       {adjust(); return TO;}
break    {adjust(); return BREAK;}
let      {adjust(); return LET;}
in       {adjust(); return IN;}
end      {adjust(); return END;}
function {adjust(); return FUNCTION;}
var      {adjust(); return VAR;}
type     {adjust(); return TYPE;}
array    {adjust(); return ARRAY;}
if       {adjust(); return IF;}
then     {adjust(); return THEN;}
else     {adjust(); return ELSE;}
do       {adjust(); return DO;}
of       {adjust(); return OF;}
nil      {adjust(); return NIL;}
{id}     {adjust(); yylval.sval = yytext ; return ID;} 
{digits} {adjust(); yylval.ival=atoi(yytext); return INT;}

.	 {adjust(); EM_error(EM_tokPos,"illegal token");}


