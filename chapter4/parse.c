/* 27.11.14 by Mkth
 * Abstract Struct Tree engine
 * input is file of tig
 * output is A_exp Tree
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "prabsyn.h"

extern int yyparse(void);
extern A_exp absyn_root;

/* parse source file fname; 
   return abstract syntax data structure */
A_exp parse(string fname) 
{
	EM_reset(fname);
    if (yyparse() == 0) /* parsing worked */
	{
		printf("parse successfully!\n");
		return absyn_root;
	}
	else 
		printf("fuck! not pass syntax!\n");
		return NULL;
}


