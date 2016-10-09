//#include <stdio.h>
//#include <string.h>
#include "intercept.h"
#include "isa.h"
#include <stdio.h>
#include <string.h>

#define N 100

int main(int argc, char ** argv)
{
	int i,j;
	float a[N][N];
	int x = 0;
	pjz_before_main(argv[0]);

	printf("Hello,world!\n");	

	pjz_after_main();

	return 0;
}
