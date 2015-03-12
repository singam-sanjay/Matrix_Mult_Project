#include<stdio.h>

#define B  1
#define KB (1024*B)

size_t sizeof_L3 = (size_t)4096 * KB;

int main()
{
	size_t sizeof_row = 1024*sizeof(double);
	int n = 10;

	while( n<=15 )
	{
		printf("N=2^%i (SizeOfRow %6zuB:%3zukB) can have %3zu rows in L3 cache\n",n,sizeof_row,sizeof_row/KB,sizeof_L3/sizeof_row);
		n += 1;
		sizeof_row *= 2;
	}

	return 0;
}
