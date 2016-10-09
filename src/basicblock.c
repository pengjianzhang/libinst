#include  <elf.h>
#include "global.h"
#include "isa.h"
#include "map.h"
#include "basicblock.h"
#include "binary.h"
#include "codecache.h"
#include <stdio.h>

Elf32_Sym * current_fun;

/******Main data*********************/
/*
struct bb
{
	ADDRESS start;	//basic block begin virtual address
	int	insn_num;	// instruction numbers in this basic block
	UINT64	count;	// how many times the basic block executed
};
*/

#define	BASIC_BLOCK_SIZE	(1*MEGA_1)

struct bb all_basic_block [BASIC_BLOCK_SIZE];	// contain all the basic blocks
unsigned	all_basic_block_size = BASIC_BLOCK_SIZE; //the basic block array size
unsigned	all_basic_block_n;	//how many the bb array used	

/******Asisstant Data******************
A array based address list
Contain basic block entries
head element -- addr_list[0];
tail element -- addr_list[1];
*/
#define	ADDRESS_LIST_SIZE	10240

#define	MAX_ADDRESS		(~0)
#define MIN_ADDRESS		0
#define	NULL_INDEX		-1

struct address_list
{
	ADDRESS addr;
	int next;
};

struct address_list addr_list[ADDRESS_LIST_SIZE];
int addr_list_n = 0;	//how many element used
int list_head_index = 0;	//head of the list

#define	FOR_EACH_ELE_IN_ADDRESS_LIST(index) for(index = addr_list[0].index; addr_list[index].index != NULL_INDEX; index= addr_list[index].index)

//before mark one function, call this function to init some global data
static void pjz_init_address_list()
{

	addr_list[0].addr = MIN_ADDRESS;
	addr_list[0].next = NULL_INDEX;
//	addr_list[1].addr=MAX_ADDRESS;
//	addr_list[1].next = NULL_INDEX;

	addr_list_n = 1;
}

//after mark one function, call this function to output mark data
static void pjz_fini_address_list()
{
	int index;
	int i;
	int size_byte;
	ADDRESS	addr;
	int tmp_bb_n = all_basic_block_n;
//check free space
//all_basic_block_size; //the basic block array size
//unsigned        all_basic_block_n;      

	if(all_basic_block_n + addr_list_n > all_basic_block_size) 
	{
		printf("all_basic_block size insufficient\n"); 
		exit(1);
	}

//copy entry address
//	FOR_EACH_ELE_IN_ADDRESS_LIST(index)
	for(index = addr_list[0].next; index != NULL_INDEX; index= addr_list[index].next)
	{
		addr = addr_list[index].addr;
		all_basic_block[tmp_bb_n].start = addr;
		tmp_bb_n ++;
	}


//calculate basic block size 

	for(i = all_basic_block_n; i < tmp_bb_n -1; i++)
	{
		size_byte = all_basic_block[i+1].start -  all_basic_block[i].start;
		all_basic_block[i].insn_num = size_byte >> 2;
	}
// the last basic block's size

	if((tmp_bb_n - all_basic_block_n) == 1) 
		all_basic_block[i].insn_num = current_fun->st_size >> 2;
	else
		all_basic_block[i].insn_num = (current_fun->st_size - (all_basic_block[i].start - current_fun->st_value)) >> 2;
// (current_fun->st_value + current_fun->st_size - all_basic_block[i-1].start )>>2;
		
	all_basic_block_n = tmp_bb_n;	
}
/***********************************************/

//add an entry_addr to the address_list
//this procedure filt repeated entry addresses 
static int pjz_add_bb_entry_to_address_list(ADDRESS entry_addr)
{
	int i = 0;
	int tmp;
	int index_p = 0;
	int index_pre = 0;

	if(entry_addr < current_fun->st_value || entry_addr >= current_fun->st_value + current_fun->st_size)
		pjz_my_error("entry_error");

	//find place
	while(index_p != NULL_INDEX)
	{ 
		if(entry_addr > addr_list[index_p].addr)
		{
			index_pre = index_p;
			index_p = addr_list[index_p].next;
		}
		else break;
	} 

	//entry addr is unique
	if(entry_addr == addr_list[index_p].addr) return 1;

	if(addr_list_n >= ADDRESS_LIST_SIZE ) 
		{printf("address list size insufficient\n"); exit(1);} // need better sollution

	// insert
	addr_list[addr_list_n].addr = entry_addr;
	addr_list[addr_list_n].next = index_p;
	addr_list[index_pre].next = addr_list_n;

//	if(addr_list[addr_list_n].addr == addr_list[index_pre].addr){ printf("ERROR\n"); exit(1);}
//	if(addr_list[addr_list_n].addr == addr_list[index_p].addr){ printf("ERROR\n"); exit(1);}

	addr_list_n ++;

	return 1;
}


//add the start of the function
static void pjz_add_normal_insn(ADDRESS nor_addr, INSN_T insn)
{
	pjz_add_bb_entry_to_address_list(nor_addr);
}

//find one branch instruction, add it to a basic-block-mark-list
static void pjz_add_branch_insn(ADDRESS br_addr, INSN_T insn)
{
	ADDRESS target1,target2;
	target1 = pjz_branch_target(br_addr, insn);
	target2 = br_addr + 8;
//	printf("target\t%x\t%x\n",br_addr,target1,target2);

	if(target1 >= current_fun->st_value && target1 < current_fun->st_value + current_fun->st_size)
	{
		pjz_add_bb_entry_to_address_list(target1);
	}
	if(target2 >= current_fun->st_value && target2 < current_fun->st_value + current_fun->st_size)
	{
		pjz_add_bb_entry_to_address_list(target2);
	}
}



void pjz_init_pjz_mark_bb()
{
//	all_basic_block = get_space(4*MEGA_1,256*MEGA_1, 10*MEGA_1);
//	all_basic_block = pjz_get_space_in_any_place(10*MEGA_1);
//	all_basic_block_size = 10*MEGA_1/sizeof(struct bb);	
//	all_basic_block = (struct bb *) malloc(BASIC_BLOCK_SIZE * sizeof(struct bb));
	all_basic_block_n = 0;
}

//mark basic block in one function -- current function
static void pjz_pjz_mark_bb_in_function()
{
	INSN_T insn;
	ADDRESS addr;
	//add this start address of the current function
	pjz_init_address_list();
	pjz_add_bb_entry_to_address_list(current_fun->st_value);


	FOR_EACH_INSN_IN_FUN(current_fun, addr) 	
	{
		
		insn = *((unsigned *)addr);
		if(pjz_is_branch(insn)) {
			pjz_add_branch_insn(addr,insn);
		}
	}

//	printf(" d\n");
	pjz_fini_address_list();
//	printf(" e\n");
}
/*
	ADDRESS start;	//basic block begin virtual address
	int	insn_num;	// instruction numbers in this basic block
//	struct bb * next_1;
//	struct bb * next_2;
	INSN_T i1;
	INSN_T i2;
	ADDRESS addr;
	UINT64	counter;	// how many times the basic block executed
};
*/

//fill i1 i2 addr  type fild of basic block
static void pjz_fill_an_basicblock(struct bb * bb_p)
{
	INSN_T i1,i2;
	INSN_CLASS c,c1,c2;
	BB_CLASS bc;
	ADDRESS addr = bb_p -> start;
	bb_p -> i1 = 0;
	bb_p -> i2 = 0;
	bb_p -> in_point = 0;
	bb_p -> counter = 0;
	if(bb_p -> insn_num == 1) {bb_p -> type = BB_SINGLE; return;}
	else if(bb_p -> insn_num < 1) { pjz_my_error("basic block size < 1");}
	
	i1 = INSN_GET(addr);
	i2 = INSN_GET(addr + 4);
	c1 = pjz_get_insn_type(i1); 
	c2 = pjz_get_insn_type(i2);
	
	if((c1 == NORMAL) && (c2 != NORMAL)) addr += 4;

	bb_p -> in_point = addr;	
	bb_p->i1 = INSN_GET(addr);
	bb_p ->i2 =  INSN_GET(addr + 4);
	c = pjz_get_insn_type(i1);
	switch(c)
	{
		case NORMAL:	bc = BB_NORMAL;break;
		case JR_LINK:	bc = BB_JR_LINK;break;
		case JR_NO_LINK:	bc = BB_JR_NO_LINK;break;
		case B_LINK:	bc = BB_B_LINK;break;
		case B_NO_LINK:	bc = BB_B_NO_LINK;break;
		case J_LINK:	bc = BB_J_LINK;break;
		case J_NO_LINK:	bc = BB_J_NO_LINK;break;
		default: pjz_my_error("pjz_fill_an_basicblock\n");
	}
	bb_p->type = bc;
}

static void pjz_fill_all_basicblock()
{
	struct bb * bb_p;
	int i;
	
	FOR_EACH_BB(bb_p,i)
	{
		pjz_fill_an_basicblock(bb_p);
	}
	pjz_cc_flush();
} 

static void pjz_recover_an_basicblock(struct bb * bb_p)
{
	ADDRESS addr = bb_p->in_point;
	if(addr)
	{
		INSN_AT(addr) = bb_p -> i1;
		INSN_AT(addr+4) = bb_p -> i2;
	}
}

void pjz_recover_all_basicblock()
{
	struct bb * bb_p;
	int i;
	
	FOR_EACH_BB(bb_p,i)
	{
		pjz_recover_an_basicblock(bb_p);
	}

}


//mark all the basic block
void pjz_mark_bb()
{
	Elf32_Sym * fun;

	FOR_EACH_FUNCTION(fun)
	{
		current_fun = fun;
//		printf("mark function %s\n", Executable.pStringTable + current_fun->st_name);
		pjz_pjz_mark_bb_in_function();
	}
//	printf("basic block nums = %d\n", all_basic_block_n);

	pjz_fill_all_basicblock();
}

//**************Debug Function*************************

void pjz_print_bb_insn_num()
{
	struct bb * bb_p;
	int i;
	FILE * fp;
	fp = fopen("bb_counter.txt","w");
	FOR_EACH_BB(bb_p,i)
	{
//		printf("%x\t%d\t%x\t%x\t%x\t%ld\n",bb_p->start,bb_p -> insn_num,bb_p->in_point, bb_p->i1,bb_p->i2,bb_p -> counter);
//		printf("%x\t%d\t%d\n",bb_p -> in_point,bb_p->insn_num,bb_p -> counter);
		fprintf(fp,"%x\t%x\t%d\n",bb_p ->start,bb_p->start + bb_p->insn_num * 4,bb_p -> counter);
//		printf("%d\n",bb_p -> counter);
	}
	fclose(fp);
}

void pjz_print_bb(struct bb * bb_p)
{
	printf("start %x\tinsn_num %d\tin_point %x\ti1 %x\ti2 %x\tcounter %ld\n",bb_p->start,bb_p -> insn_num,bb_p->in_point, bb_p->i1,bb_p->i2,bb_p -> counter);
}
