/*counting_insn.c
 *
 */

#include "global.h"
#include "isa.h"
#include "stack.h"
#include "codecache.h"
#include "basicblock.h"

//br_insn have two reg position
static void pjz_find_2_free_reg(INSN_T br_insn, REG_T regs[])
{
	int i = 1;
	REG_T reg1,reg2, reg3,t1,t2;
		

	reg1 = S_REG(br_insn);   
	reg2 = T_REG(br_insn);
	reg3 = D_REG(br_insn);

	
	while(1)
	{
		if((i != reg1) && (i != reg2) && i != reg3){ t1 = i; break;}
		i++;
	}	    

	while(1)
	{	
		i++;
		if((i != reg1) && (i != reg2) && (i != reg3)){ t2 = i; break;}
	}

	regs[0] = t1;
	regs[1] = t2;
}


static void pjz_pjz_find_2_free_reg_for_br(INSN_T i1, INSN_T i2,REG_T regs[])
{
	int i,j;
	REG_T r1,r2,r3,r4,r5,r6;
	char r_reg[R_REG_NUM];
	r1 = S_REG(i1);   
	r2 = T_REG(i1);
	r3 = D_REG(i1);

	r4 = S_REG(i2);   
	r5 = T_REG(i2);
	r6 = D_REG(i2);
	
	for(i = 0; i< R_REG_NUM;i++)
		r_reg[i] = 0;

	r_reg[r1] = 1;
	r_reg[r2] = 1;
	r_reg[r3] = 1;
	r_reg[r4] = 1;
	r_reg[r5] = 1;
	r_reg[r6] = 1;

	i = 0;
	j = 1;
	while(i != 2)
	{
		if(r_reg[j] == 0) regs[i++] = j; 
		j++;
	}	
}


//fill CC with insns, which like asm"inc addr"
int pjz_inc_insns(REG_T b,REG_T t,INSN_T cc[],ADDRESS addr)
{
	cc[0] = pjz_make_lui(b,addr>>16);
	cc[1] = pjz_make_inc_x(b,addr&0xffff);
	cc[2] = pjz_make_lw(b,t,0);
	cc[3] = pjz_make_inc_w(t);
//	cc[4] = 0;
	cc[4] = pjz_make_sw(b,t,0);
	cc[5] = pjz_make_sw(b,t,0);
	cc[6] = 0;
	cc[7] = 0;	
	return 8;
}


#define	NORMAL_CODE_SIZE 24	


//generate code for normal basic block
static void pjz_instrument_NORMAL(ADDRESS start, UINT32 * counter)
{
	INSN_T normal[NORMAL_CODE_SIZE]; 
	ADDRESS bb_start = start;
	INSN_T	first =  INSN_AT(bb_start);// *((unsigned *) bb_start);
	INSN_T  second =  INSN_AT(bb_start + 4);//*((unsigned *) (bb_start + 4));
	ADDRESS cc_addr = pjz_cc_get_top_address();
	ADDRESS counter_addr = (ADDRESS)counter;

	INSN_T push_base = pjz_make_push(BASE_REG);
	INSN_T push_tmp = pjz_make_push(TMP_REG);
	INSN_T pop_tmp = pjz_make_pop(TMP_REG);
	INSN_T pop_base = pjz_make_pop( BASE_REG);
//jump to code cache	
//	*((unsigned *) bb_start) 
	INSN_AT(bb_start) = pjz_make_j(bb_start,cc_addr);
//	*((unsigned *) (bb_start + 4)) 
	INSN_AT(bb_start + 4)= 0;
//adjust normal code

//	printf("normal %x\n",start);	

	normal[0] = first;//push_base;//pjz_make_push(BASE_REG);
	normal[1] = second;//push_tmp;//pjz_make_push(TMP_REG);
	normal[2] = push_base;//pop_tmp;//pjz_make_pop(TMP_REG);
	normal[3] = push_tmp;//pop_base;//pjz_make_pop(BASE_REG);
/*
	normal[4] = pjz_make_lui(BASE_REG,counter_addr>>16);
	normal[5] = pjz_make_inc_x(BASE_REG,counter_addr&0xffff);
	normal[6] = pjz_make_lw(BASE_REG,TMP_REG,0);
	normal[7] = pjz_make_inc_w(TMP_REG);
	normal[8] = pjz_make_sw(BASE_REG,TMP_REG,0);
*/

	int offset = pjz_inc_insns(BASE_REG,TMP_REG,&(normal[4]),counter_addr);

	offset += 4;
	normal[offset + 0] = pop_tmp;
	normal[offset + 1] = pop_base;
	normal[offset + 2] = pjz_make_j(cc_addr + (offset+2) * 4,bb_start + 8);
	normal[offset + 3] = 0;
/*
	normal[0] = first;//push_base;//pjz_make_push(BASE_REG);
	normal[1] = second;//push_tmp;//pjz_make_push(TMP_REG);
	normal[2] = push_base;//pop_tmp;//pjz_make_pop(TMP_REG);
	normal[3] = push_tmp;//pop_base;//pjz_make_pop(BASE_REG);
	normal[4] = 0;
	normal[5] = pjz_make_lui(BASE_REG,counter_addr>>16);
	normal[6] = pjz_make_inc_x(BASE_REG,counter_addr&0xffff);
	normal[7] = pjz_make_lw(BASE_REG,TMP_REG,0);
	normal[8] = pjz_make_inc_w(TMP_REG);
	normal[9] = 0;
	normal[10] = pjz_make_sw(BASE_REG,TMP_REG,0);
	normal[11] = 0;
	normal[12] = 0;
	normal[13] = 0;//13;
	normal[14] = 0;
	normal[15] = pop_tmp;
	normal[16] = pop_base;
	normal[17] = 0;
	normal[18] = pjz_make_j(cc_addr + 18 * 4,bb_start + 8);
	normal[19] = 0;
*/
/*
	normal[0] = push_base;//pjz_make_push(BASE_REG);
	normal[1] = push_tmp;//pjz_make_push(TMP_REG);
	normal[2] = pjz_make_lui(BASE_REG,counter_addr>>16);
	normal[3] = pjz_make_ld(BASE_REG,TMP_REG,counter_addr&0xffff);
	normal[4] = pjz_make_inc_d(TMP_REG);
	normal[5] = pjz_make_sd(BASE_REG,TMP_REG,counter_addr&0xffff);
	normal[6] = pop_tmp;//pjz_make_pop(TMP_REG);
	normal[7] = pop_base;//pjz_make_pop(BASE_REG);
	normal[8] = first;
	normal[9] = second;
	normal[10] = 0;
	normal[11] = pjz_make_j(cc_addr + 44,bb_start + 8);
	normal[12] = 0;
*/
//*/

/*
	printf("------------\n");
	int i;
	for(i=0; i<10;i++)
		printf("%x\n",normal[i]);
	printf("++-------");
*/	

//copy normal --> codecache
	pjz_cc_add_code(normal,NORMAL_CODE_SIZE);
}

/*counting_branch.c
 *
 *counting each basic block
 */

/*
branch instruction have two types
1, slot always executed
2, slot executed when branch taken

so we bind branch with slot to make things easy
*/

#define	BEGIN_BRANCH_SIZE 20

static void pjz_instrument_B_NO_LINK(ADDRESS point, UINT64 * counter )
{ 
	INSN_T begin_branch[BEGIN_BRANCH_SIZE];
	REG_T regs[2];
	REG_T base_reg,tmp_reg;
	ADDRESS bb_start = point;
	INSN_T br_insn = INSN_AT(bb_start); //* ((unsigned *) bb_start);
	INSN_T br_slot = INSN_AT(bb_start +4);//* ((unsigned *) (bb_start + 4));
		
	ADDRESS cc_addr = pjz_cc_get_top_address();
	ADDRESS counter_addr = counter;	

	ADDRESS taken_target_addr = pjz_branch_target(bb_start, br_insn);
	ADDRESS untaken_target_addr = bb_start + 8; 

	pjz_pjz_find_2_free_reg_for_br(br_insn,br_slot, regs);	
	base_reg = regs[0];
	tmp_reg = regs[1];

//jump to codecache	
//	*((unsigned *) bb_start) 
	INSN_AT(bb_start)= pjz_make_j(bb_start,cc_addr);	
//	*((unsigned *) 
	INSN_AT(bb_start+4) = INSN_NOP;	
	
//gen code
	INSN_T push_base = pjz_make_push(base_reg);
	INSN_T push_tmp =  pjz_make_push(tmp_reg);
	INSN_T pop_tmp = pjz_make_pop(tmp_reg);
	INSN_T pop_base = pjz_make_pop(base_reg);

	begin_branch[0] = push_base;
	begin_branch[1] = push_tmp;

	int offset = pjz_inc_insns(base_reg,tmp_reg,&(begin_branch[2]),counter_addr);
/*
	begin_branch[2] = pjz_make_lui(base_reg,counter_addr>>16);;	//lui
	begin_branch[3] = pjz_make_ld(base_reg,tmp_reg,counter_addr&0xffff);	//ld
	begin_branch[4] = pjz_make_inc_d(tmp_reg);
	begin_branch[5] = pjz_make_sd(base_reg,tmp_reg,counter_addr&0xffff);	//sd
*/	
	offset += 2;
	begin_branch[offset + 0] = pop_tmp;	
	begin_branch[offset + 1] = pop_base;	
	begin_branch[offset + 2] = pjz_adjust_branch(br_insn, 3);	//branch
	begin_branch[offset + 3] = br_slot;
	begin_branch[offset + 4] = pjz_make_j(cc_addr + 4*(offset + 4), untaken_target_addr);	//j
	begin_branch[offset + 5] = INSN_NOP;	
	begin_branch[offset + 6] = pjz_make_j(cc_addr + 4 * (offset + 6), taken_target_addr);	//j
	begin_branch[offset + 7] = INSN_NOP;	
//copy to codecache

	pjz_cc_add_code(begin_branch, BEGIN_BRANCH_SIZE);	
}
/*counting_return.c
 * instrument j, jr
 */


#define	J_INSN_SIZE 16


static void pjz_instrument_j_jr(ADDRESS point, UINT64 * counter )
{ 
	
	INSN_T begin_branch[J_INSN_SIZE];
	REG_T regs[2];
	REG_T base_reg,tmp_reg;
	ADDRESS bb_start = point;
	INSN_T j_insn = * ((unsigned *) bb_start);
	INSN_T br_slot = * ((unsigned *) (bb_start + 4));
		
	ADDRESS cc_addr = pjz_cc_get_top_address();
	ADDRESS counter_addr = counter;	


	pjz_pjz_find_2_free_reg_for_br(j_insn,br_slot, regs);	
	base_reg = regs[0];
	tmp_reg = regs[1];

//jump to codecache	
	*((unsigned *) bb_start) = pjz_make_j(bb_start,cc_addr);	
//	*((unsigned *) (bb_start+4)) = INSN_NOP;	
	
//gen code
	INSN_T push_base = pjz_make_push(base_reg);
	INSN_T push_tmp =  pjz_make_push(tmp_reg);
	INSN_T pop_tmp = pjz_make_pop(tmp_reg);
	INSN_T pop_base = pjz_make_pop(base_reg);

	begin_branch[0] = push_base;
	begin_branch[1] = push_tmp;

	int offset = pjz_inc_insns(base_reg,tmp_reg,&(begin_branch[2]),counter_addr);
/*
	begin_branch[2] = pjz_make_lui(base_reg,counter_addr>>16);;	//lui
	begin_branch[3] = pjz_make_ld(base_reg,tmp_reg,counter_addr&0xffff);	//ld
	begin_branch[4] = pjz_make_inc_d(tmp_reg);
	begin_branch[5] = pjz_make_sd(base_reg,tmp_reg,counter_addr&0xffff);	//sd
*/	
	offset += 2;
	begin_branch[offset + 0] = pop_tmp;	
	begin_branch[offset + 1] = pop_base;	
	begin_branch[offset + 2] = j_insn;	//j
	begin_branch[offset + 3] = INSN_NOP;	
//copy to codecache

	pjz_cc_add_code(begin_branch, J_INSN_SIZE);	
}


static void pjz_instrument_JR_NO_LINK(ADDRESS point, UINT64 * counter )
{
	pjz_instrument_j_jr(point,counter );
}

static void pjz_instrument_J_NO_LINK(ADDRESS point, UINT64 * counter )
{
	pjz_instrument_j_jr(point,counter );
}
/*counting_call.c
 *
 *call instruction have two types
 *1: funtion address in register
 *2: Use pc + offset to pointer an function address
 * The return address must be store in an register
 */

#define	CALL_SIZE	16	


//jalr
//
//jump to codecache
//j
//old slot

//codecache

//push
//push
//lui
//ld
//inc
//sd
//li d,j ; lui d, hi16(j) ; ori d,d, lo16(j)
//mv addr->ar
//pop
//pop
//jr dd
//nop



//for jalr ...
static void pjz_instrument_JR_LINK(ADDRESS start,  UINT64 * counter)
{
	
//	ADDRESS bb_start = start;	
	INSN_T call_insn[CALL_SIZE];
	ADDRESS cc_addr = pjz_cc_get_top_address();
	ADDRESS counter_addr = counter;	
	ADDRESS	return_addr = start + 8;
	INSN_T insn = INSN_GET(start);
	REG_T regs[2];
	
		
//use return address reg as base_reg		
	pjz_find_2_free_reg(insn,regs);
	REG_T base_reg = D_REG(insn); 
	REG_T tmp_reg = regs[1];
	INSN_T push_tmp = pjz_make_push(tmp_reg);
	INSN_T pop_tmp = pjz_make_pop(tmp_reg);


//jump to codecache	
	*((unsigned *) start) = pjz_make_j(start,cc_addr);	
	
//gen code

	call_insn[0] = push_tmp;
	
	int offset = pjz_inc_insns(base_reg,tmp_reg,&(call_insn[1]),counter_addr);
/*
	call_insn[1] = pjz_make_lui(base_reg,counter_addr>>16);;	//lui
	call_insn[2] = pjz_make_ld(base_reg,tmp_reg,counter_addr&0xffff);	//ld
	call_insn[3] = pjz_make_inc_d(tmp_reg);
	call_insn[4] = pjz_make_sd(base_reg,tmp_reg,counter_addr&0xffff);	//sd
*/	
	offset += 1;
	call_insn[offset + 0] = pjz_make_lui(base_reg,return_addr >> 16);
	call_insn[offset + 1] = pjz_pjz_make_ori(base_reg,base_reg,return_addr & 0xffff);
	call_insn[offset + 2] = pjz_pjz_make_jr(S_REG(insn));
	call_insn[offset + 3] = pop_tmp;		

	
//copy to codecache

	pjz_cc_add_code(call_insn, CALL_SIZE);	
}

static void pjz_instrument_J_LINK(ADDRESS start,  UINT64 * counter)
{
	
//	ADDRESS bb_start = start;	
	INSN_T call_insn[CALL_SIZE];
	ADDRESS cc_addr = pjz_cc_get_top_address();
	ADDRESS counter_addr = counter;	
	ADDRESS	return_addr = start + 8;
	INSN_T insn = INSN_GET(start);
	REG_T regs[2];
	
		
//use return address reg as base_reg		
	pjz_find_2_free_reg(insn,regs);
	REG_T base_reg = REG_RA; 
	REG_T tmp_reg = regs[1];
	INSN_T push_tmp = pjz_make_push(tmp_reg);
	INSN_T pop_tmp = pjz_make_pop(tmp_reg);


//jump to codecache	
	*((unsigned *) start) = pjz_make_j(start,cc_addr);	
	
//gen code
/*
	call_insn[0] = 0;//push_tmp;
	call_insn[1] = 0;//pjz_make_lui(base_reg,counter_addr>>16);;	//lui
	call_insn[2] = 0;//pjz_make_ld(base_reg,tmp_reg,counter_addr&0xffff);	//ld
	call_insn[3] = 0;//pjz_make_inc_d(tmp_reg);
	call_insn[4] = 0;//pjz_make_sd(base_reg,tmp_reg,counter_addr&0xffff);	//sd
	
	call_insn[5] = 0;//pjz_make_lui(base_reg,return_addr >> 16);
	call_insn[6] = 0;//pjz_pjz_make_ori(base_reg,base_reg,return_addr & 0xffff);
	call_insn[7] = insn;
	call_insn[8] = 0;//pop_tmp;		

*/	
	call_insn[0] = push_tmp;

	int offset = pjz_inc_insns(base_reg,tmp_reg,&(call_insn[1]),counter_addr);
/*
	call_insn[1] = pjz_make_lui(base_reg,counter_addr>>16);;	//lui
	call_insn[2] = pjz_make_ld(base_reg,tmp_reg,counter_addr&0xffff);	//ld
	call_insn[3] = pjz_make_inc_d(tmp_reg);
	call_insn[4] = pjz_make_sd(base_reg,tmp_reg,counter_addr&0xffff);	//sd
*/	offset += 1;
	call_insn[offset + 0] = pjz_make_lui(base_reg,return_addr >> 16);
	call_insn[offset + 1] = pjz_pjz_make_ori(base_reg,base_reg,return_addr & 0xffff);
	call_insn[offset + 2] = pjz_adjust_jal_to_j(insn);
	call_insn[offset + 3] = pop_tmp;		

//*/
//copy to codecache

	pjz_cc_add_code(call_insn, CALL_SIZE);	
}




#define	CALL_OFF_SIZE	24	
//for bal ...
/*
	j cc
---------------
	sd tmp
	sd base
	lui
	ld
	inc_d
	sd
	lui 
	ori 
	ld tmp
	bal ...
	nop
	mv base ra
	j return addr
	ld base	
	mv bsee ra
	j function
	ld base
*/
static void pjz_instrument_B_LINK(ADDRESS start,   UINT64 * counter)
{
	
//	ADDRESS bb_start = start;	
	INSN_T call_insn[CALL_OFF_SIZE];
	ADDRESS cc_addr = pjz_cc_get_top_address();
	ADDRESS counter_addr = counter;	
	ADDRESS	return_addr = start + 8;
	INSN_T insn = INSN_GET(start);
	REG_T regs[2];
	ADDRESS function_addr = pjz_branch_target(start,insn);	
//	insn = pjz_adjust_branch(insn, );		
//use return address reg as base_reg		
	pjz_find_2_free_reg(insn,regs);
	REG_T base_reg = regs[0]; 
	REG_T tmp_reg = regs[1];
	INSN_T push_base = pjz_make_push(base_reg);
	INSN_T push_tmp = pjz_make_push(tmp_reg);
	INSN_T pop_tmp = pjz_make_pop(tmp_reg);
	INSN_T pop_base = pjz_make_pop(base_reg);

//jump to codecache	
	*((unsigned *) start) = pjz_make_j(start,cc_addr);	
	
//gen code

	call_insn[0] = push_base;
	call_insn[1] = push_tmp;

	int offset = 	pjz_inc_insns(base_reg,tmp_reg,&(call_insn[2]),counter_addr);
/*
	call_insn[2] = pjz_make_lui(base_reg,counter_addr>>16);;	//lui
	call_insn[3] = pjz_make_ld(base_reg,tmp_reg,counter_addr&0xffff);	//ld
	call_insn[4] = pjz_make_inc_d(tmp_reg);
	call_insn[5] = pjz_make_sd(base_reg,tmp_reg,counter_addr&0xffff);	//sd
*/	
	offset += 2;
	call_insn[offset + 0] = pjz_make_lui(base_reg,return_addr >> 16);
	call_insn[offset + 1] = pjz_pjz_make_ori(base_reg,base_reg,return_addr & 0xffff);

	call_insn[offset + 2] = pop_tmp; 
	call_insn[offset + 3] = pjz_adjust_branch(insn, 4);		
	call_insn[offset + 4] = INSN_NOP;
	call_insn[offset + 5] = make_mv(REG_RA,base_reg);	
	call_insn[offset + 6] = pjz_make_j( cc_addr +  4 * (offset + 6), return_addr );	
	call_insn[offset + 7] = pop_base;
	
	call_insn[offset + 8] = make_mv(REG_RA,base_reg);	
	call_insn[offset + 9] = pjz_make_j( cc_addr + 4*(offset + 9) , function_addr );	
	call_insn[offset + 10] = pop_base;
	
//copy to codecache

	pjz_cc_add_code(call_insn, CALL_OFF_SIZE);	
}

void pjz_instrument_insn(INSN_CLASS i_class, ADDRESS start, UINT64 * counter)
{
	INSN_T insn = INSN_AT(start);
	INSN_CLASS c = pjz_get_insn_type(insn);

	if(c != i_class) 
	{
		printf("bb %d new get %d\n",c,i_class);
		pjz_my_error("pjz_instrument_insn INSN_CLASS unequal");

	}
	switch (i_class)
	{
		case NORMAL:	//printf("normal\n");
				pjz_instrument_NORMAL(start, counter);
					break;
		case JR_LINK:	//printf("JR_LINK\n");
				pjz_instrument_JR_LINK(start, counter);
				break;
		case JR_NO_LINK: //printf("JR_NO_LINK\n");
				pjz_instrument_JR_NO_LINK(start, counter );
				break;
		case B_LINK:	//printf("B_LINK\n");
				pjz_instrument_B_LINK(start, counter);
				break;
		case B_NO_LINK:	//printf("B_NO_LINK\n");
				pjz_instrument_B_NO_LINK(start,counter );
				break;
		case J_LINK:	//printf("J_LINK\n");
				pjz_instrument_J_LINK(start, counter);
				break;
		case J_NO_LINK:	//printf("J_NO_LINK\n");
				pjz_instrument_J_NO_LINK(start,counter );
				break;
		default: 	printf("UNKNOWN\n");pjz_my_error("pjz_instrument_insn unknow INSN_CLASS");
	}
}

void pjz_instrument_insn_2(INSN_CLASS i_class, struct bb * bb_p)
{
	ADDRESS start = bb_p -> in_point;
	ADDRESS counter =(ADDRESS) &(bb_p -> counter);
//	INSN_T insn = INSN_AT(start);
	INSN_T insn = INSN_GET(start);
	INSN_CLASS c = pjz_get_insn_type(insn);

	if(c != i_class) 
	{
		printf("bb %d new get %d addr %x insn %x\n",c,i_class, start,insn);
		
		pjz_print_bb(bb_p);
//		pjz_my_error("pjz_instrument_insn INSN_CLASS unequal");
		asm("break");
	//	return ;
	}
//	return ;
//	bb_p -> counter++;
	switch (i_class)
	{
		case NORMAL:	//printf("normal\n");
					pjz_instrument_NORMAL(start, counter);
					break;
		case JR_LINK:	//printf("JR_LINK\n");
				//pjz_instrument_JR_LINK(start, counter);
				break;
		case JR_NO_LINK: //printf("JR_NO_LINK\n");
				//pjz_instrument_JR_NO_LINK(start, counter );
				break;
		case B_LINK:	//printf("B_LINK\n");
				//pjz_instrument_B_LINK(start, counter);
				break;
		case B_NO_LINK:	//printf("B_NO_LINK\n");
				//pjz_instrument_B_NO_LINK(start,counter );
				break;
		case J_LINK:	//printf("J_LINK\n");
				//pjz_instrument_J_LINK(start, counter);
				break;
		case J_NO_LINK:	//printf("J_NO_LINK\n");
				//pjz_instrument_J_NO_LINK(start,counter );
				break;
		default: 	printf("UNKNOWN\n");pjz_my_error("pjz_instrument_insn unknow INSN_CLASS");
	}
}
