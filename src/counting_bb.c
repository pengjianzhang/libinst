#include "counting_insn.h"
#include "basicblock.h"
#include "global.h"

/*
struct bb
{
	ADDRESS start;	//basic block begin virtual address
	int	insn_num;	// instruction numbers in this basic block
	struct bb * next_1;
	struct bb * next_2;
	UINT64	counter;	// how many times the basic block executed
};


void pjz_instrument_insn(INSN_CLASS i_class, ADDRESS start, UINT64 * counter);
*/

//we instrument the first 2-3 instructions
//so we use this instruction INSN_CLASS type to diside how to instrument
static INSN_CLASS pjz_bb_type(struct bb * bb_p , ADDRESS * in_addr)
{
	
	INSN_T i1,i2;
	INSN_CLASS c1,c2;
	ADDRESS addr = bb_p -> start;
	if(bb_p -> insn_num == 1){ * in_addr = addr;	return NORMAL;}
	
//	printf("%x\n",addr);
	i1 = INSN_GET(addr);
	i2 = INSN_GET(addr + 4);
	c1 = pjz_get_insn_type(i1); 
	c2 = pjz_get_insn_type(i2);
	
	if(c1!=NORMAL) {*in_addr = addr; return c1;}
	else
	{
		if(c2 == NORMAL) {*in_addr = addr; return NORMAL;}
		else {*in_addr = addr + 4; return c2;}
	}
}


//instrument an basic block
//if INSN_NUM == 1 dont't instrument
//else instrument by it's type
static void pjz_instrument_a_bb(struct bb * bb_p)
{
	
	ADDRESS addr;
	INSN_CLASS c =  //pjz_get_insn_type(bb_p->i1);//
			pjz_bb_type(bb_p, &addr);
	
	if(bb_p->insn_num != 1) //pjz_instrument_insn_2(c,bb_p);
		pjz_instrument_insn(c, addr, &(bb_p->counter));
}


//instrument all basic blocks
void pjz_instrument_bbs()
{
	int i;
	struct bb * bb_p;
	FOR_EACH_BB(bb_p,i)
	{
//		if(i == 5) break;
		pjz_instrument_a_bb(bb_p);
	}
}
