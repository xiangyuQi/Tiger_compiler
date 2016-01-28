a.out: parsetest.o  parse.o  prabsyn.o tiger.tab.o lex.yy.o errormsg.o util.o table.o absyn.o symbol.o semant.o types.o env.o 
	cc -g parsetest.o  parse.o  prabsyn.o tiger.tab.o lex.yy.o errormsg.o util.o table.o absyn.o symbol.o semant.o types.o env.o 

types.o: types.c types.h
	cc -g -c types.c
	
env.o: env.c env.h
	cc -g -c env.c
	
semant.o: semant.c semant.h 
	cc -g -c semant.c
	
parsetest.o: parsetest.c parse.h prabsyn.h semant.h 
	cc -g -c parsetest.c
	
parse.o: parse.c parse.h absyn.h errormsg.h util.h
	cc -g -c parse.c
	
tiger.tab.o: tiger.tab.c
	cc -g -c tiger.tab.c

tiger.tab.c: tiger.y
	bison -dv tiger.y

tiger.tab.h: tiger.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

symbol.o: symbol.c symbol.h util.h table.h
	cc -g -c symbol.c
	
absyn.o: absyn.c util.h symbol.h absyn.h
	cc -g -c absyn.c

table.o: table.c table.h util.h
	cc -g -c table.c
	
prabsyn.o: prabsyn.c prabsyn.h symbol.h util.h absyn.h
	cc -g -c prabsyn.c
	
errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c tiger.tab.h errormsg.h util.h
	cc -g -c lex.yy.c

lex.yy.c: tiger.lex
	flex tiger.lex

util.o: util.c util.h
	cc -g -c util.c
	
	
clean: 
	rm -f a.exe a.out util.o parsetest.o lex.yy.o errormsg.o tiger.tab.c tiger.tab.h tiger.tab.o parse.o symbol.o table.o absyn.o prabsyn.o lex.yy.c lex.yy.o types.o env.o semant.o
