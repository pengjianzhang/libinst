#ifndef BASIC_BLOCK
#define	BASIC_BLOCK

#include "global.h"

typedef enum{ 
	BB_SINGLE,	
        BB_NORMAL,         //normal instruction 
        BB_JR_LINK,        //jump and link , jalr
        BB_JR_NO_LINK,     //jump , jr
        BB_B_LINK,         //branch and link, bal
        BB_B_NO_LINK,      //normal branch instruction , bne
        BB_J_LINK,         //jal
        BB_J_NO_LINK,      //jump , j
} BB_CLASS;



struct bb
{
//	UINT64	counter;	// how many times the basic block executed
	UINT32	counter;	// how many times the basic block executed
	ADDRESS start;	//basic block begin virtual address
	int	insn_num;	// instruction numbers in this basic block
//	struct bb * next_1;
//	struct bb * next_2;
	INSN_T i1;
	INSN_T i2;
	ADDRESS in_point;
	BB_CLASS type;
//	UINT64	counter;	// how many times the basic block executed
};

extern struct bb all_basic_block[];	// contain all the basic blocks
//struct bb * all_basic_block;	// contain all the basic blocks
extern unsigned	all_basic_block_size; //the basic block array size
extern unsigned	all_basic_block_n;	//how many the bb array used	


void pjz_init_pjz_mark_bb();

void pjz_mark_bb();

#define FOR_EACH_BB(bb,i) for(bb = all_basic_block+0,i=0; i< all_basic_block_n; ++i,bb = all_basic_block+i)

void pjz_print_bb_insn_num();
void pjz_print_bb(struct bb * bb_p);
void pjz_recover_all_basicblock();

#endif
