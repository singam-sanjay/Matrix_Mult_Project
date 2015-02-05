#include<stdio.h>
#include<assert.h>
#include<math.h>

int main(int argc, char** argv)
{
	if( argc != 4 )
	{
		perror("I can accept 4 arguments only.\nint file file");
		assert( argc == 4 );
	}

	register int N=atoi(argv[1]);
	register FILE *one,*two;
	one = fopen(argv[2],"r");
	two = fopen(argv[3],"r");

	double ONE,TWO;
	register double *oNE,*tWO;
	oNE = &ONE;
	tWO = &TWO;

	register int iter1,iter2;
	for( iter1=0 ; iter1<N ; iter1+=1 )
	{
		for( iter2=0 ; iter2<N ; iter2+=1 )
		{
			fscanf(one,"%lf",oNE);
			fscanf(two,"%lf",tWO);

			if( fabs(ONE-TWO) > 10.0e-8 )
			{
				fprintf(stderr,"[%i,%i]\n",iter1,iter2);
			}
		}
	}
	
	fclose(one);
	fclose(two);
	return 0;
}
