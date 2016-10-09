/*stack.c
 *
 *manage stack
 */
#include "isa.h"
#include "map.h"

unsigned long long * stack;
int sp;



#define	PUSH_INSNS	1	//PUSH need how many instructions
#define	POP_INSNS	1	//POP need how many instructions
#define	SECOND_PAGE	(0x4000)
#define	STACK_START_ADDR  SECOND_PAGE

//init stack 
//allocate space to stack,which is the 2nd page of process space, and init some asisstant data structure
void pjz_init_stack()
{
	stack = pjz_get_space_at_address(STACK_START_ADDR, pagesize);
	
//	printf("%x\n",stack);

	 sp = 0;
}


// make push instructions, and store the instructions to CODE, from REG; return instruction nums
INSN_T pjz_make_push(REG_T reg)
{
	INSN_T insn;
	ADDRESS addr = &(stack[sp]);
	
	insn = pjz_make_sd(ZERO_REG,reg,addr);
		
	sp++;
	return  insn;
}


//make pop instructions to REG, and store the instructions to CODE, return instruction nums
INSN_T pjz_make_pop(REG_T reg)
{
	INSN_T insn;
	ADDRESS	addr;
	if(sp == 0) {printf("pjz_make_pop ERROR: stack empty\n"); exit(1);}
	addr = &(stack[sp-1]);
	
	insn = pjz_make_ld(ZERO_REG,reg,addr);
	sp --;

	return insn;
}



