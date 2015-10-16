#include "util.h"
#include "slp.h"
#include "prog1.h"

//a := 5+3; b := print (a, (a-1), 10*a); print (b);
A_stm prog(void) 
{
    return 
    A_CompoundStm(
	    A_AssignStm("a",A_OpExp(A_NumExp(5), A_plus, A_NumExp(3))),   
		A_CompoundStm(
			A_AssignStm(
				"b",
				A_EseqExp(
					A_PrintStm(
						A_PairExpList(
							A_IdExp("a"),
							A_LastExpList(A_OpExp(A_IdExp("a"), A_minus, A_NumExp(1)))
						)
					),
					A_OpExp(A_NumExp(10), A_times, A_IdExp("a"))
			    )
			),
			A_PrintStm(A_LastExpList(A_IdExp("b")))
		)
   );
}
// print(a,print(print(c,5),0),a);
A_stm prog2(void)
{
	  return A_PrintStm(A_PairExpList(A_IdExp("a"),
	     A_PairExpList(
	    	A_EseqExp(A_PrintStm(A_LastExpList
			   (A_EseqExp(A_PrintStm(A_LastExpList(
			          A_IdExp("c"))),A_NumExp(5)))),A_IdExp("0")),
	       A_LastExpList(A_IdExp("a"))
                )));
}

/* print (1,2,3,(print(1,2,3,4), 6)) */
A_stm prog3(void)
{
	return
	A_PrintStm(
		A_PairExpList(
			A_NumExp(1),
			A_PairExpList(
				A_NumExp(2),
				A_PairExpList(
					A_NumExp(3),
					A_LastExpList(
						A_EseqExp(
							A_PrintStm(
								A_PairExpList(
									A_NumExp(1),
									A_PairExpList(
										A_NumExp(2),
										A_PairExpList(
											A_NumExp(3),
											A_LastExpList(A_NumExp(4))
										)
									)
								)
							),
							A_NumExp(6)
						)
					)
				)
			)
		)
	);
}
// 统计表达式中的print
int margs(A_expList exps)
{
	int count = 0;
	if(exps->kind == A_pairExpList)
	{
		A_exp e = exps->u.pair.head;
		if(e->kind ==A_eseqExp)
		{
		   count += maxargs(e->u.eseq.stm);
		}
		count += margs(exps->u.pair.tail);
	}
	if(exps->kind == A_lastExpList)
	{
	    A_exp e = exps->u.last;
		if(e->kind ==A_eseqExp)
		{
		   count += maxargs(e->u.eseq.stm);
		}
	}
	return count;
}
// 统计语句中的print 
int maxargs(A_stm s)
{
	int count = 0;
	if(s->kind ==A_compoundStm)return maxargs(s->u.compound.stm1)+maxargs(s->u.compound.stm2);
	if(s->kind ==A_assignStm)
	{
		A_exp e = s->u.assign.exp;
		if(e->kind ==A_eseqExp)
		{
		   count += maxargs(e->u.eseq.stm);
		}
	
	}
	if(s->kind == A_printStm)
	{
		count++;
		A_expList el = s->u.print.exps;
        count+= margs(el);
	}
	return count;
}

Table_ 	Table(string id, int value,struct table *tail)
{
	Table_ t = checked_malloc(sizeof(*t));
	t->id = id;
	t->value = value;
	t->tail = tail;
	return t;
};

Table_  update (Table_ t,string id,int value)
{
	Table_ temp = checked_malloc(sizeof(*temp));
	temp->id = id;
	temp->value = value;
	temp->tail = t;
	return temp;
}
// 查找 key在table中对应的value 
int lookup(Table_ t,string key)
{
	Table_ temp = t;
	while(temp)
	{
		if(key == temp->id)
		{
			return temp->value;
		}
		temp = temp->tail;
	}
	assert("undef iditify");
}
struct IntAndTable interpExp(A_exp,Table_);

//解析语句 
Table_ interpStm(A_stm s,Table_ t)
{
	Table_ t1,t2;
	struct IntAndTable iat ;
	A_expList el;
	switch(s->kind)
	{
		case A_compoundStm:
			t1 = interpStm(s->u.compound.stm1,t);
			t2 = interpStm(s->u.compound.stm2,t1);
			return t2;
		case A_assignStm:
			iat = interpExp(s->u.assign.exp,t);
		    t1 = update(iat.t,s->u.assign.id,iat.i);
		    return t1;
		case A_printStm:
			el = s->u.print.exps;
			t1 = t;
		    while(el->kind == A_pairExpList)
		    {
		    	iat = interpExp(el->u.pair.head,t1);
		    	printf("%d ",iat.i);
		    	t1 = iat.t;
		    	el = el->u.pair.tail;
		    }
		    if ( el->kind == A_lastExpList)
			{
				iat = interpExp(el->u.last,t1);
				printf("%d ",iat.i);
			}
			return iat.t ;
	}
	return t;
}
// 解析表达式 
struct IntAndTable interpExp(A_exp e,Table_ t)
{
   Table_ t1;
   struct IntAndTable iat1,iat2;	
   switch(e->kind)
   {
   	  case A_idExp:
   	  	 iat1.i = lookup(t,e->u.id);
   	  	 iat1.t = t;
   	  	 return iat1;
   	  case A_numExp:
   	  	 iat1.i = e->u.num;
   	  	 iat1.t = t;
   	  	 return iat1;
   	  case A_opExp :
		 iat1 = interpExp(e->u.op.left,t);
		 iat2 = interpExp(e->u.op.right,iat1.t);
		 switch(e->u.op.oper)
		 {
		 	case A_plus :
		 	  iat2.i = iat1.i + iat2.i;
		 	  return iat2;
		 	case A_minus :
		 	  iat2.i = iat1.i - iat2.i;
		 	  return iat2;
		 	case A_times :
			  iat2.i = iat1.i * iat2.i;
			  return iat2;
			case A_div :
			  iat2.i = iat1.i / iat2.i;
			  return iat2; 
		 }
	  case A_eseqExp:
	      t1 = interpStm(e->u.eseq.stm,t);
		  iat1 = interpExp(e->u.eseq.exp,t1);
		  return iat1;	 	 
		 	 
   }	
}
void interp(A_stm s)
{
	Table_ t = 0;
	interpStm(s,t);
	printf("\n");
}

