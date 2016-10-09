/*codecache.c
 *
 *Manage of codecache
 */
#include "map.h"
#include "global.h"
#include <asm/cachectl.h>   // cacheflush
#include <sys/mman.h>   // mprotect, mmap

/*
static unsigned * codecache;
static unsigned codecache_size;
static unsigned codecache_n;
*/

#define	CODECACHE_SIZE	(1*MEGA_1)  //(10*MEGA_1)

unsigned codecache[CODECACHE_SIZE];
unsigned codecache_size;
unsigned codecache_n;


static void pjz_print_code(unsigned * code, int len)
{
	int i;
	printf("\n\n*******\n");
	for(i = 0; i< len; i++)
		printf("%x\n",code[i]);
}


void pjz_cc_init()
{
//	codecache = pjz_get_space_in_range(4*MEGA_1,256*MEGA_1, CODECACHE_SIZE);
//	printf("codecache %x %d\n",codecache, (unsigned)codecache/(1024*1024));
	codecache_size = CODECACHE_SIZE ; //sizeof(unsigned);
	codecache_n = 0;

    ADDRESS pPageStart = (unsigned)codecache&~(pagesize-1);
    mprotect((void*)pPageStart, codecache_size * sizeof(unsigned) - pPageStart, PROT_READ | PROT_WRITE | PROT_EXEC);

}

void pjz_cc_add_code(unsigned * code, unsigned len)
{
	int i;
	if(codecache_n + len > codecache_size) {printf("add_code ERROR, codecache overflow\n");exit(1);}
	for(i=0; i < len; i++ )
		codecache[codecache_n + i] = code[i];
	codecache_n += len;	

//	pjz_print_code(code,len);
}

//get the top address of codecache
ADDRESS pjz_cc_get_top_address()
{
	return (ADDRESS)(&(codecache[codecache_n]));
}

//flush codecache
void pjz_cc_flush()
{
	ADDRESS pPageStart = (unsigned)codecache&~(pagesize-1);
	mprotect((void*)pPageStart, codecache_size * sizeof(unsigned) - pPageStart, PROT_READ | PROT_WRITE | PROT_EXEC);
	cacheflush(codecache, codecache_size * sizeof(unsigned),BCACHE);
}


