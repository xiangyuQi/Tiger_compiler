#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "prabsyn.h"
#include "semant.h"
int main(int argc, char **argv)
{
	extern int yydebug;
  	yydebug = 1;

  	if (argc!=2) {
    fprintf(stderr,"usage: a.out filename\n");
    exit(1);
  	}
 // pr_exp(stdout, parse(argv[1]), 4);
 	A_exp temp = parse(argv[1]);
	if (temp) {
		SEM_transProg(temp);
	}

  return 0;
}                                     
