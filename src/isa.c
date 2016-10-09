#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "isa.h"

/* 
typedef enum{ 
	NORMAL,		//normal instruction 
	JR_LINK, 	//jump and link , jalr
	JR_NO_LINK	//jump , jr
	B_LINK, 	//branch and link, bal
	B_NO_LINK, 	//normal branch instruction , bne
	J_LINK,		//jal
	J_NO_LINK,	//jump , j
} INSN_CLASS;
*/

INSN_CLASS pjz_get_insn_type(INSN_T insn)
{
	int main_op = MAIN_OP(insn);
	int sub_op = SUB_OP(insn);
	int s_reg = S_REG(insn);
	int t_reg = T_REG(insn);

	if(main_op == 0) 
	{
		if(sub_op == 8) return JR_NO_LINK;
		else if(sub_op == 9) return JR_LINK;
		else return NORMAL;
	}
	else if( main_op == 1 )
	{	
		if( (t_reg <= 3) || t_reg == 18 || t_reg== 19) return B_NO_LINK;  //bltz bgez bltzl bgezl bltzall bgezall
		else if ( (t_reg == 16) ||  (t_reg == 17) ) return B_LINK; // bltzal bgezal 
		else return NORMAL;
	}
	else if(main_op == 2) return J_NO_LINK;
	else if(main_op == 3) return J_LINK;
	else if(main_op <= 7) return B_NO_LINK;	// beq bne blez bgtz 
	else if(main_op <= 15) return NORMAL;	
	else if(main_op == 16)
	{
		if(s_reg == 8 && t_reg <= 3 ) return B_NO_LINK; //bc0f bc0t bc0f1 bc0t1
		else return NORMAL;
	}
	else if(main_op == 17)
	{
		if(s_reg >=8 && s_reg <= 10) return B_NO_LINK; // bc1f bc1t bc1f1 bc1t1 bc1any2f ...
		else return NORMAL;
	}
	else if(main_op == 18 )
	{
		if(s_reg == 8 && t_reg <= 3) return B_NO_LINK; //bc2f bc2t bc2f1 bc2t1
		else return NORMAL;
	}
	else if(main_op == 19) return NORMAL;
	else if(main_op <= 23) return B_NO_LINK; //beql bnel blezl bgtzl
	else return NORMAL;


}


int pjz_is_branch(INSN_T insn)
{
	if (pjz_get_insn_type(insn) == B_NO_LINK)
	{
//		printf("branch %x\n",insn);
		return 1;
	}
	else return 0;
}


inline INSN_T pjz_make_j(ADDRESS pc, ADDRESS target)
{
	INSN_T insn = 0;

	if((pc >> 28) != (target >> 28) )
	{
		printf("Error in pjz_make_j\n");
		exit(1);
	}

	insn = ((target&0x0fffffff) >> 2)|0x08000000;

	return insn;
}

inline INSN_T pjz_adjust_jal_to_j(INSN_T jal)
{
	INSN_T j;

	j = (jal &0x03ffffff) | 0x08000000;

	return j;
}


inline INSN_T pjz_pjz_make_jr(REG_T s)
{
	INSN_T insn = 0;
	
	insn = INSN_S_REG(s) | 8;

	return insn;
}


inline INSN_T pjz_make_nop()
{
	return 0;
}

inline INSN_T pjz_make_inc_x(REG_T s,unsigned x)
{
	INSN_T insn = 0;
		
	insn = INSN_MAIN_OP(13) | INSN_S_REG(s) | INSN_T_REG(s) | (x&(0xffff));	
	return insn;
}




inline INSN_T pjz_make_inc_w(REG_T s)
{
	INSN_T insn = 0;
		
	insn = INSN_MAIN_OP(9) | INSN_S_REG(s) | INSN_T_REG(s) | 0x1;	
	return insn;
}

inline INSN_T pjz_make_inc_d(REG_T s)
{
	INSN_T insn = 0;
		
	insn = INSN_MAIN_OP(25) | INSN_S_REG(s) | INSN_T_REG(s) | 0x1;	
//	insn = INSN_MAIN_OP(9) | INSN_S_REG(s) | INSN_T_REG(s) | 0x1;	
	return insn;
}

inline INSN_T pjz_make_sw(REG_T  b, REG_T t, unsigned offset)
{
	INSN_T insn = 0;

	insn = INSN_MAIN_OP(43) | INSN_S_REG(b) | INSN_T_REG(t) | offset;

	return insn;
}

inline INSN_T pjz_make_lw(REG_T b, REG_T t, unsigned offset)
{
	INSN_T insn = 0;

	insn = INSN_MAIN_OP(35) | INSN_S_REG(b) | INSN_T_REG(t) | offset;

	return insn;

}

inline INSN_T pjz_make_ld(REG_T b, REG_T t, unsigned offset)
{
	INSN_T insn = 0;

	insn = INSN_MAIN_OP(55) | INSN_S_REG(b) | INSN_T_REG(t) | offset;
//	insn = INSN_MAIN_OP(35) | INSN_S_REG(b) | INSN_T_REG(t) | offset;

	return insn;
}


inline INSN_T pjz_make_sd(REG_T b, REG_T t, unsigned offset)
{
	INSN_T insn = 0;

	insn = INSN_MAIN_OP(63) | INSN_S_REG(b) | INSN_T_REG(t) | offset;
//	insn = INSN_MAIN_OP(43) | INSN_S_REG(b) | INSN_T_REG(t) | offset;

	return insn;
}

inline INSN_T pjz_make_lui(REG_T d, unsigned c)
{
	INSN_T	insn = 0;
	INSN_T i1,i2,i3;
	
	i1 = INSN_MAIN_OP(15);
	i2 = INSN_T_REG(d) ;
	i3 =  (c&0xffff);
	insn = i1 | i2 | i3;
	return insn;
}

inline INSN_T pjz_pjz_make_ori(REG_T d, REG_T s, unsigned c)
{
	INSN_T	insn = 0;
	
	insn = INSN_MAIN_OP(13 )| INSN_S_REG(s) | INSN_T_REG(d) | c;

	return insn;
}

inline INSN_T pjz_make_or(REG_T d, REG_T s, REG_T t)
{
	INSN_T	insn = 0;
	
	insn = INSN_MAIN_OP(0 )|INSN_S_REG(s) | INSN_T_REG(t) |INSN_D_REG(d) | 37;

	return insn;
}


inline ADDRESS pjz_offset_cvt(ADDRESS offset)
{
	ADDRESS mask = 0xffff0000;
	ADDRESS ad;
	
	if((offset >> 15) == 1) offset = mask| offset;      

	offset++;
	ad = offset*4;
	
	return ad;
}


inline ADDRESS pjz_branch_target(ADDRESS pc, INSN_T br)
{
	return pc + pjz_offset_cvt(br & 0xffff);
}

inline INSN_T pjz_adjust_branch(INSN_T br, ADDRESS broffset)
{
        return (br &0xffff0000) | broffset;
}


