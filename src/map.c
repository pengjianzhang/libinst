#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "global.h"

#define	DATA_PROT (PROT_WRITE | PROT_READ)
#define	CODE_PROT  (PROT_WRITE | PROT_READ | PROT_EXEC)



#define	FLAG_SPACE_YES	1
#define	FLAG_SPACE_NO	0

//get a SIZE byte space belong to [start , end ]
void * pjz_get_space_in_range(ADDRESS start,ADDRESS end, unsigned size)
{
	int fd = open("/dev/zero",O_RDWR);
	unsigned i = 0;
	void * addr = 0;
	int step;
	int flag = FLAG_SPACE_NO;

	size = ((size + pagesize -1)/pagesize) * pagesize;
	start = ((start + pagesize -1)/pagesize ) * pagesize;
	end = ((end + pagesize -1)/pagesize) * pagesize;

	if(size >= MEGA_1) step = MEGA_1;
	else step = pagesize;

	if(start == 0)  start = step;
	
	while(start < end )
	{
		addr =  mmap((void *)start, size, DATA_PROT, MAP_SHARED ,fd, 0);

		if((ADDRESS)addr == start) {flag = FLAG_SPACE_YES;break;} 

		start += step;
	}

	close(fd);

	if(FLAG_SPACE_NO) {printf("pjz_get_space_in_range ERROR: NO SPACE for size = %x start = %x end = %x\n",size,start,end); exit(1);}

	return addr;
}

//if START == 0, map at ramdom place
void * pjz_get_space_at_address(ADDRESS start,unsigned size)
{
	int fd = open("/dev/zero",O_RDWR);
	void * addr = 0;

	size = ((size + pagesize -1)/pagesize) * pagesize;

	addr =  mmap((void *)start, size, DATA_PROT, MAP_SHARED ,fd, 0);

	close(fd);

	if(addr == MAP_FAILED ) {printf("pjz_get_space_in_any_place ERROR: NO SPACE for size = %x\n",size); exit(1);}

	return addr;

}


void * pjz_get_space_in_any_place(unsigned size)
{

	return pjz_get_space_at_address(NULL,size);
}

/*
//size must be aligned by pagesize
static void * get_map(int start_addr, unsigned size, int DIRC)
{
	int fd = open("/dev/zero",O_RDWR);
	unsigned i = 0;
	void * addr = 0;
	int step;

	if(size % pagesize != 0 ) {printf("get_map: size unligned\n"); exit(1);} 

	//step = size / 4
	step = size / pagesize;
	if(step >= 4) step = (step / 4) * pagesize;
	else step = pagesize;
	
	if(DIR_DOWN) step = step * -1;;

	if(start_addr == 0)  addr = mmap(NULL,size,DATA_PROT,MAP_SHARED,fd,0);
	else 

	while(start_addr > 0)
	{
		addr =  mmap((void *)start_addr, size, DATA_PROT, MAP_SHARED ,fd, 0);
		if(addr != MAP_FAILED){
			if(addr == (void *)start_addr ){printf("mmap sucess!\n"); break;}
			else munmap(addr, size);
		}	
		start_addr += step;
		i++;
	}

	close(fd);
//	printf("start = %x addr = %x i = %d\n",start_addr,(unsigned)addr, i);

	return addr;
}



void map_space()
{

	int code_page = ((branch_num * COUNTING_INSNS_SIZE * 4 ) + pagesize - 1) / pagesize;
	int data_page = (branch_num * 2 * 4 + pagesize -1)/pagesize;

	code_size = code_page * pagesize;
	data_size = data_page * pagesize;
	stack_size = pagesize;

	stack = get_map(pagesize,pagesize,DIR_UP); 
	code_cache = get_map(0x400000,code_size,DIR_DOWN);
	branch_counter = get_map(0,data_size,DIR_NON);

	printf("space acess test start\n");
//	code_cache[0] = 100;
//	stack[0] = 10;
//	branch_counter[5] = 2;
	printf("space acess test end\n");

	if(((void *) stack == MAP_FAILED) ||( (void * ) code_cache == MAP_FAILED) || ((void *)branch_counter == MAP_FAILED))
	{
		printf("map-space error\n");
		exit(1);
	}

	mprotect(code_cache,code_size, CODE_PROT);

//	print_counter2(10);	

}




void unmap_space()
{
	munmap((void *)stack, stack_size); 
	munmap((void *)code_cache, code_size); 
	munmap((void *)branch_counter,data_size);
}


void  put_num_counter()
{
	int i;

	
	for( i = 0; i < 20; i++)
		branch_counter[i] = 0;

}

void print_counter()
{
	int i;

	for( i = 0; i < branch_counter_n ; i++)
		printf("%d \n", branch_counter[i]);

}
*/
