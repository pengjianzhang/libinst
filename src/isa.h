#ifndef ISA_H
#define	ISA_H

#include "global.h"
 
 
typedef enum{ 
	NORMAL,		//normal instruction 
	JR_LINK, 	//jump and link , jalr
	JR_NO_LINK,	//jump , jr
	B_LINK, 	//branch and link, bal
	B_NO_LINK, 	//normal branch instruction , bne
	J_LINK,		//jal
	J_NO_LINK,	//jump , j
} INSN_CLASS;


#define	R_REG_NUM	32
#define	REG_RA	31

#define	REG_ZERO	0
#define	INSN_NOP	0

#define MAIN_OP(insn)   ((insn) >> 26) 
#define SUB_OP(insn)    ((insn) & 0x3f)
#define EXT_OP(insn)    (((insn) >> 21) & 0x1f)
#define S_REG(insn)     (((insn) >> 21) & 0x1f) 
#define T_REG(insn)     (((insn) >> 16) & 0x1f)
#define D_REG(insn)     (((insn) >> 11) & 0x1f)
#define SHIFT(insn)     (((insn) >> 6) & 0x1f)
#define BROFFSET(insn)  ((insn) &0xffff)
#define TARGET(insn)    ((insn) &0x3ffffff)


#define	INSN_MAIN_OP(op)	((op)<<26)
#define	INSN_S_REG(s)		((s)<<21)
#define	INSN_T_REG(t)		((t)<<16)
#define	INSN_D_REG(d)		((d)<<11)


#define	INSN_GET(addr) ((INSN_T)(*((unsigned *)(addr))))
#define INSN_AT(addr) (*((unsigned *)(addr)))

int pjz_is_branch(INSN_T insn);
INSN_CLASS pjz_get_insn_type(INSN_T insn);

inline INSN_T pjz_make_j(ADDRESS pc, ADDRESS target);

inline INSN_T pjz_adjust_jal_to_j(INSN_T jal);

inline INSN_T pjz_pjz_make_jr(REG_T s);

inline INSN_T pjz_make_nop();

inline INSN_T pjz_make_inc_x(REG_T s,unsigned x);
inline INSN_T pjz_make_inc_w(REG_T s);
inline INSN_T pjz_make_inc_d(REG_T s);

inline INSN_T pjz_make_sw(REG_T  b, REG_T t, unsigned offset);

inline INSN_T pjz_make_lw(REG_T b, REG_T t, unsigned offset);

inline INSN_T pjz_make_sd(REG_T  b, REG_T t, unsigned offset);

inline INSN_T pjz_make_ld(REG_T b, REG_T t, unsigned offset);


inline INSN_T pjz_make_lui(REG_T d, unsigned c);

inline INSN_T pjz_pjz_make_ori(REG_T d, REG_T s, unsigned c);

inline INSN_T pjz_make_or(REG_T d, REG_T s, REG_T t);

#define	make_mv(d,t) pjz_make_or(d,REG_ZERO,t)

inline ADDRESS pjz_branch_target(ADDRESS pc, INSN_T br);

inline INSN_T addjust_branch(INSN_T br, ADDRESS broffset);

#endif
