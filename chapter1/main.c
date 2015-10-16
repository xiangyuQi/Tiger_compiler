#include "util.h"
#include "slp.h"
#include "prog1.h"

int main()
{
	/*
	int num;
	num = maxargs(prog2());
	printf("%d\n", num);
	*/
	A_stm s = prog();
	interp(s);
	interp(prog2());
	interp(prog3());
	getchar();
	return 0;
}

