typedef struct table *Table_;
struct table 
{
	string id;
	int value;
	Table_ tail;
};

struct IntAndTable
{
	int i;
	Table_ t;
};
A_stm prog(void);
A_stm prog2(void);
int maxargs(A_stm );
void interp(A_stm) ;


