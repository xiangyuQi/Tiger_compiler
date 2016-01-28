#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "translate.h"
#include "env.h"
#include "semant.h"
 
struct expty transVar(S_table venv, S_table tenv, A_var v);
struct expty transExp(S_table venv, S_table tenv, A_exp e);
void 		 transDec(S_table venv,	S_table tenv, A_dec d);
Ty_ty        transTy(				S_table tenv, A_ty ty);
Ty_tyList makeFormalTyList(S_table tenv , A_fieldList fl);
Ty_fieldList makeFieldLists(S_table tenv, A_fieldList fl);
bool argsMatch(S_table venv,S_table tenv,A_expList el,Ty_tyList tl,A_exp fun);
bool tyMatch(Ty_ty t1,Ty_ty t2);
bool efieldsMatch(S_table venv, S_table tenv,Ty_ty ty,A_exp e);
Ty_ty actual_ty(Ty_ty ty);

void SEM_transProg(A_exp exp)
{
	transExp(E_base_venv(),E_base_tenv(),exp);
}

struct expty transVar(S_table venv, S_table tenv, A_var v)
{
	switch(v->kind)  
	{
	
		case A_simpleVar:  /* var id (a)*/
		{	
			E_enventry x = S_look(venv,v->u.simple);
			if(x && x->kind == E_varEntry)
				return expTy(NULL,actual_ty(x->u.var.ty));
			else
			{
				EM_error(v->pos,"undefined variable %s",S_name(v->u.simple));
				return expTy(NULL,Ty_Int());
			}
		}	
		case A_fieldVar:
		{	
			struct expty et = transVar(venv, tenv, v->u.field.var);
      		if (et.ty && et.ty->kind == Ty_record)
		    {
       			 Ty_fieldList fl = et.ty->u.record;
        		 for(; fl; fl = fl->tail) 
				 {
          		 	if (fl->head->name == v->u.field.sym) 
            		return expTy(NULL, actual_ty(fl->head->ty));
        		 }
      		}
      		// can NOT get the v's name, use field name instaed
     		EM_error(v->pos, "undefined variable %s",S_name(v->u.field.sym));
      		return expTy(NULL, Ty_Record(NULL));
      	}
		case A_subscriptVar: 
		{
     		 struct expty et = transVar(venv, tenv, v->u.subscript.var);
     		 if (et.ty && et.ty->kind == Ty_array)
			 {
        	  	 struct expty et2 = transExp(venv, tenv, v->u.subscript.exp);
				 if (et2.ty->kind == Ty_int)
				 	return expTy(NULL, actual_ty(et.ty->u.array));
				 else 
				    EM_error(v->pos, "int required");
			}
      		 // can NOT get v's name
      		 EM_error(v->pos, "undefined variable");
     		 return expTy(NULL, Ty_Array(NULL));
     	}
     	default:
			assert(0);	 
	}		
}

struct expty transExp(S_table venv, S_table tenv, A_exp e)
{
	switch(e->kind)
	{
	 	case A_varExp:
      		return transVar(venv, tenv, e->u.var);

   	    case A_nilExp:
      		return expTy(NULL, Ty_Nil());

   	    case A_intExp:
      		return expTy(NULL, Ty_Int());

   	    case A_stringExp:
      		return expTy(NULL, Ty_String());
// translate function call: check that arguments match the
// formal params
   		case A_callExp:
      	{
    		E_enventry callinfo;
      		callinfo = S_look(venv,e->u.call.func);
      		if(callinfo &&callinfo->kind == E_funEntry )
      		{
      			if(argsMatch(venv,tenv,e->u.call.args,callinfo->u.fun.formals,e))
      			{
      				if(callinfo->u.fun.result)
      					return expTy(NULL, actual_ty(callinfo->u.fun.result));
      				else return expTy(NULL,Ty_Void());
      			}
      		}
      		else EM_error(e->pos, "undefined function %s\n", S_name(e->u.call.func));
      			
			return expTy(NULL, Ty_Void());
		}
/* exp op exp, rules:
 * 1. +,-,*,/ only apply to integer operands
 * 2. =,<>,>,<,>=,<= apply to:
 *  integers
 *  record or array of the same type: compare pointers
 *  strings: compare contents
 */
 			  		
   	    case A_opExp:
   	    {
      		A_oper oper = e->u.op.oper;
      		struct expty left = transExp(venv,tenv,e->u.op.left);
      		struct expty right = transExp(venv,tenv,e->u.op.right);
      		switch(oper)
      		{
      			case A_plusOp:
   			    case A_minusOp:
       			case A_timesOp:
    			case A_divideOp:
	      			if (left.ty->kind != Ty_int) {
	       				 EM_error(e->u.op.left->pos, "integer required");
	      			}
	      			if (right.ty->kind != Ty_int) {
	        			EM_error(e->u.op.right->pos, "integer required");
	      			}
	      			return expTy(NULL, Ty_Int());
	      		case A_eqOp:
			    case A_neqOp:
			    case A_ltOp:
			    case A_leOp:
			    case A_gtOp:
			    case A_geOp:
			      if (!Ty_is_compatible(left.ty, right.ty)) {
			        EM_error(e->pos, "operands type mismatch");
			      }
			      return expTy(NULL, Ty_Int());	
      			default:
					assert(0);	
      		}
      		
      	}
/* type-id { id = exp {, id = exp}} or type-id {}
 * the field names and types of the record exp must match
 * those of the named type, in the order given
 */
    	case A_recordExp:
    	{
    	
      		Ty_ty recordTy = actual_ty(S_look(tenv,e->u.record.typ));
      		if(recordTy)
      		{
      			if(recordTy->kind !=Ty_record)
      			{
      				EM_error(e->pos, "%s is not a record type", S_name(e->u.record.typ));	
					return expTy(NULL, Ty_Record(NULL));
      			}
      			if(efieldsMatch(venv,tenv,recordTy,e))
      				return expTy(NULL,recordTy);
      		}
      		else 
      			EM_error(e->pos, "undefined type %s (debug recordExp)", S_name(e->u.record.typ)); 
			return expTy(NULL,Ty_Record(NULL));
		}
// evaluate the exp list from left to right, use the last
// exp's result as the result of the whole list
    	case A_seqExp:
    	{
      		A_expList list = e->u.seq;
      		if(!list)
      		{
      			return expTy(NULL, Ty_Void());
      		}
      		while (list->tail)
		    {
				transExp(venv, tenv, list->head);
				list = list->tail;
			}
      		return transExp(venv, tenv, list->head);
      	}
/* lvalue := exp
 * evaluates lvalue, then exp; set the contents of the lvalue
 * to the result of exp; check that the types of lvalue and
 * the exp are the same, but the whole assignment exp produce
 * no value, so (a := b) + c is illegal
 */
    	case A_assignExp:
    	{
      		struct expty  tvar =  transVar(venv,tenv,e->u.assign.var);
			struct expty evar =  transExp(venv,tenv,e->u.assign.exp);
			if (!tyMatch(tvar.ty, evar.ty)) 
			{
				EM_error(e->pos, "unmatched assign exp");
			}
			return expTy(NULL, Ty_Void());
		}
/* if exp1 then exp2 [else exp3]
 * 1. exp1 is integer type
 * 2. exp2 and exp3 are the same type if exp3 exists
 * 3. exp2 produce no value if exp3 does not exists
 */			
    	case A_ifExp:
    	{
      		struct expty condition = transExp(venv,tenv,e->u.iff.test);
      		struct expty then = transExp(venv,tenv,e->u.iff.then);
      		if(condition.ty->kind != Ty_int)
      			EM_error(e->u.iff.test->pos, "int required");
			else if(e->u.iff.elsee)
			{
				struct expty elsee = transExp(venv,tenv,e->u.iff.elsee);
				if(!tyMatch(then.ty,elsee.ty))
				{
				   EM_error(e->pos, "if-else sentence must return same type");
				}
			}else 
			{
    			if (then.ty != Ty_Void()) {
      			EM_error(e->pos, "then type of if is NOT void");
    			}	
    			return expTy(NULL, Ty_Void());
    		}
			return expTy(NULL, then.ty);
		}
/* while exp1 do exp2
 * 1. exp1's type is integer
 * 2. exp2 produces no value
 */
    	case A_whileExp:
    	{
      		struct expty condition = transExp(venv,tenv,e->u.whilee.test);
			if(condition.ty->kind !=Ty_int)
			{
				EM_error(e->pos, "int required");
			}
			struct expty body =transExp(venv,tenv,e->u.whilee.body);
		    if (body.ty != Ty_Void()) 
			{
    			EM_error(e->pos, "body's type of while must be void");
 			}
			return expTy(NULL,Ty_Void());
		}
/* for id := exp1 to exp2 do exp3
 * 1. both exp1 and exp2 are integer type
 * 2. exp3 is void type
 * 3. id is a new variable implicitly declared by the for 
 * statement, whose scope covers only exp3
 */
    	case A_forExp:
    	{
      		struct expty lo = transExp(venv,tenv,e->u.forr.lo);
      		struct expty hi = transExp(venv,tenv,e->u.forr.hi);
      		struct expty body;
      		if(lo.ty!= Ty_Int() || hi.ty!=Ty_Int())
      		{
      			EM_error(e->pos, "low or high range type is not integer");
      		}
			S_beginScope(venv);
			transDec(venv,tenv,A_VarDec(e->pos, e->u.forr.var, S_Symbol("int"), e->u.forr.lo));
			body = transExp(venv, tenv, e->u.forr.body);
			 if (body.ty != Ty_Void()) {
   				 EM_error(e->pos, "body of for is not void");
  			}
			S_endScope(venv);
			return expTy(NULL,Ty_Void());
		}
    	case A_breakExp:
      		return expTy(NULL, Ty_Void());
/* let decs in expseq end
 * the result of the last exp of expseq is that of the entire
 * let-exp
 */
    	case A_letExp:
    	{
      		A_decList decs;
      		struct expty exp ;
			S_beginScope(venv);
			S_beginScope(tenv);
			for (decs = e->u.let.decs; decs; decs = decs->tail) 
			{
				transDec(venv, tenv, decs->head);
			}
			exp = transExp(venv, tenv, e->u.let.body);
			S_endScope(venv);
			S_endScope(tenv);
			return exp;
		}
/* type-id[exp1] of exp2
 * 1. type-id is array
 * 2. exp1 is integer
 */
    	case A_arrayExp:
    		{
      		struct expty size = transExp(venv, tenv, e->u.array.size);
  			struct expty init = transExp(venv, tenv, e->u.array.init);
  			Ty_ty typ =S_look(tenv, e->u.array.typ);
  		
  			if(!typ)
  			{
  					EM_error(e->pos, "undefined type %s (debug recordExp)", S_name(e->u.record.typ)); 
  			}else
  			{
  				if (size.ty != Ty_Int()) 
				{
    				EM_error(e->pos, "type of array size is not integer");
 				}
  				if(typ->kind !=Ty_array)
  				{
  					EM_error(e->pos, "%s is not a array", S_name(e->u.record.typ));	
  				}
  				if (efieldsMatch(venv, tenv, typ, e))
			    {
					return expTy(NULL, typ);
				}
  			}
  			return expTy(NULL,Ty_Record(NULL));
  		}
	}
}
void transDec(S_table venv,	S_table tenv, A_dec d)
{
	switch (d->kind)
	{
		case A_varDec:
		{
			struct expty e = transExp(venv,tenv,d->u.var.init);
			if (d->u.var.typ) {
    		/* 1. check the type constraint if any
     		 * 2. init value of type Ty_Nil must be constrainted 
			 * by a Ty_Record type
     		*/
    		Ty_ty expected_ty = S_look(tenv, d->u.var.typ);
   			 if (!expected_ty)
			 {
      			EM_error(d->pos, "undefined type %s", S_name(d->u.var.typ));
     			return;
    		 }

   			 if ((expected_ty->kind != Ty_record && e.ty == Ty_Nil())
        				|| !tyMatch(expected_ty, e.ty))
			 {
      				EM_error(d->pos, "inconsistent var type %s",S_name(d->u.var.var));
      				return;
    		 }
  			}
			S_enter(venv,d->u.var.var,E_VarEntry(e.ty));
			break;
		}
		case  A_functionDec:
		{
			A_fundecList fcl; 
		    Ty_ty resultTy;
		    Ty_tyList formalTys,s;
		    A_fundec f = fcl->head;
			A_fieldList l;
			E_enventry fun;
			struct expty e;
			for(fcl = d->u.function;fcl;fcl = fcl->tail)
			{
				if (fcl->head->result)
				{
					resultTy = S_look(tenv,fcl->head->result);
					if(!resultTy)
					{
						EM_error(fcl->head->pos, "undefined type for return type");
						resultTy = Ty_Void();
					}
				}
				else
				{
					resultTy = Ty_Void();
				}
			    formalTys = makeFormalTyList(tenv, fcl->head->params);
			    S_enter(venv,fcl->head->name,E_FunEntry(formalTys,resultTy));
			}
			for(fcl = d->u.function;fcl;fcl = fcl->tail)
			{
				f = fcl->head;
				
				S_beginScope(venv);
				formalTys = makeFormalTyList(tenv, f->params);
				for (l = f->params, s = formalTys; l && s; l = l->tail, s = s->tail) {
					S_enter(venv, l->head->name, E_VarEntry(s->head));
				}
				e = transExp(venv, tenv, f->body);
				fun = S_look(venv, f->name);
				if (!tyMatch(fun->u.fun.result, e.ty)) {
					EM_error(f->pos, "incorrect return type in function '%s'", S_name(f->name));
				}
				S_endScope(venv);
			}
		}
	}
}

Ty_ty  transTy(S_table tenv, A_ty ty)
{
	Ty_ty final ;
	Ty_fieldList tfl;
	switch(ty->kind)
	{
		case A_nameTy:
			final = S_look(tenv,ty->u.name);
			if(!final)
			{
				EM_error(ty->pos,"undefined type %s", S_name(ty->u.name));
				return Ty_Int();
			}
			return final;
		case A_recordTy:
			tfl = makeFieldLists(tenv,ty->u.record);
			return Ty_Record(tfl);
		case A_arrayTy:
			final = S_look(tenv,ty->u.array);
			if(!final)
			{
				EM_error(ty->pos,"undefined type %s", S_name(ty->u.array));
			}
			return Ty_Array(final);
		default:
			assert(0);
	}
}

Ty_fieldList makeFieldLists(S_table tenv, A_fieldList fl)
{
	Ty_ty ty;
	A_fieldList afl =fl;
	Ty_fieldList final = NULL ,head = NULL;
	Ty_field t;
	for (;afl;afl=afl->tail)
	{
		ty = S_look(tenv,afl->head->typ);
		if(!ty)
		{
				EM_error(afl->head->pos,"undefined type %s", S_name(afl->head->typ));
		}
		else
		{
			t = Ty_Field(afl->head->name,ty);
			if(!final)
			{
				final = Ty_FieldList(t,NULL);
				head = final;
			}else
			{
				final->tail = Ty_FieldList(t,NULL);
				final = final->tail;
			}
		}
		
	}
	return head;
}

Ty_tyList makeFormalTyList(S_table t , A_fieldList fl)
{
	Ty_ty ty;
	A_fieldList afl = fl;
	Ty_tyList final = NULL,head = NULL;
	for (;afl;afl = afl->tail)
	{
		ty = S_look(t,afl->head->typ);
		if(!ty)
		{
			EM_error(afl->head->pos,"undefined type %s", S_name(afl->head->typ));
			ty = Ty_Int();
		}
		if(!final)
		{
			final = Ty_TyList(ty,NULL);
			head = final;
		}else
		{
			final->tail = Ty_TyList(ty,NULL);
			final = final->tail;
		}
	}
	return head;
}
bool tyMatch(Ty_ty t1,Ty_ty t2)
{
	Ty_ty tt1 = actual_ty(t1);
	Ty_ty tt2 = actual_ty(t2);
	int tk1 = tt1->kind;
	int tk2 = tt2->kind;
	return (((tk1 == Ty_record || tk1 == Ty_array) && tt1 == tt2) ||
			 (tk1 == Ty_record && tk2 == Ty_nil) ||
			 (tk2 == Ty_record && tk1 == Ty_nil) ||
			 (tk1 != Ty_record && tk1 != Ty_array && tk1 == tk2));
}

bool argsMatch(S_table venv,S_table tenv,A_expList el,Ty_tyList tl,A_exp fun)
{
	struct expty t;
	A_expList ell = el;
	Ty_tyList tll = tl;
	while(ell && tll)
	{
		t = transExp(venv,tenv,ell->head);
		if (!tyMatch(t.ty,tll->head))
		{
			EM_error(fun->pos, "unmatched params in function %s\n", S_name(fun->u.call.func));
			return FALSE;
		}
		ell = ell->tail;
		tll = tll->tail;
	}
	if (ell && !tll)
	{
		EM_error(fun->pos,"too many params in function %s\n", S_name(fun->u.call.func));
		return FALSE;
	}else if(!ell && tll) 
	{
		EM_error(fun->pos, "less params in function %s\n", S_name(fun->u.call.func));
		return FALSE;
	}else
	{
		return TRUE;
	}
}
bool efieldsMatch(S_table venv, S_table tenv,Ty_ty ty,A_exp e)
{
	struct expty et;
	Ty_fieldList tfl = ty->u.record;
	A_efieldList efl = e->u.record.fields;
	while (tfl &&efl)
	{
		et = transExp(venv,tenv,efl->head->exp);
		if( tfl->head->name != efl->head->name ||!tyMatch(tfl->head->ty,et.ty))
		{
			EM_error(e->pos, "unmatched name: type in %s", S_name(e->u.record.typ));
			return FALSE;
		}
		tfl = tfl->tail;
		efl = efl->tail;
	}
	if(tfl &&!efl)
	{
		EM_error(e->pos, "less fields in %s", S_name(e->u.record.typ));
		return FALSE;
	}else if (!tfl && efl)
	{
		EM_error(e->pos, "too many field in %s", S_name(e->u.record.typ));
		return FALSE;
	}
	return TRUE;
}
Ty_ty actual_ty(Ty_ty ty)
{
	if (!ty) return ty;
	if (ty->kind == Ty_name) actual_ty(ty->u.name.ty);
	else return ty;
}


