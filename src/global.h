#ifndef GLOBAL_H
#define	GLOBAL_H


// 1 M, 256 M
#define	MEGA_1	(1024*1024)
#define	JUMP_RANGE	(256*MEGA_1)


//register used in maked instruction
#define	BASE_REG	12
#define	TMP_REG		13
#define	ZERO_REG	0



// types

typedef unsigned ADDRESS;
typedef unsigned UINT32	;
typedef int	 INT32;
typedef int	 INDEX;
typedef unsigned INSN_T;
typedef long long 	UINT64;


typedef int	 REG_T;

extern int pagesize;

void pjz_global_env_init();

void pjz_my_error(char * msg);
#endif
