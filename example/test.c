//#include <stdio.h>
//#include <string.h>
//#include "intercept.h"
//#include "isa.h"
#include <stdio.h>
#include <string.h>
#include "insn.h"

#define N 100

int main(int argc, char ** argv)
{
	int i,j,k;
	int x = 0;
	float t;
	float a[N][N],b[N][N],c[N][N];
	pjz_before_main(argv[0]);

	for(i = 0; i < N; i++)	
	for(j = 0; j< N; j++)
	{
		t = 0;
		for(k=0;k<N;k++)
			t += a[i][k] * b[k][j];	
		c[i][j] = t;
	}	
	pjz_after_main();

	return 0;
}
