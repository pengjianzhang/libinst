#include "global.h"
#include "binary.h"
#include "stack.h"
#include "codecache.h"
#include "counting_bb.h"



//Before user program, this function be called
void pjz_Optimize(int fp) 
{
	pjz_global_env_init();
	pjz_IintExecutable(fp);

//************DEBUG**********
//	pjz_dump_function();
//	pjz_pjz_dump_function2();
	
	pjz_init_stack();	//init stack
	pjz_cc_init();	// init codecache
	pjz_init_pjz_mark_bb();	// init pjz_mark_bb
	pjz_mark_bb();

//	pjz_print_bb_insn_num();
//	pjz_executable_flush();
//	pjz_cc_flush();


	pjz_instrument_bbs();
//	pjz_executable_flush();
//	pjz_cc_flush();

}

//After user program, this function be called

void pjz_Collection()
{
	

//	pjz_print_bb_insn_num();
	pjz_recover_all_basicblock();
//	print_counter();
//	unmap_space();
//	printf("*****IN pjz_Collection****\n");	
	pjz_print_bb_insn_num();
}

