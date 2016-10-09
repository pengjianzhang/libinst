
#include <unistd.h>

int pagesize;

void pjz_global_env_init()
{
	pagesize = getpagesize();
}

//while in instrument, we'd better don't use glibc function
void pjz_my_error(char * msg)
{
	printf("%s%c",msg,'\n');
	exit(-1);
}
