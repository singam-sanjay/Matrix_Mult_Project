#include<stdio.h>
#include<assert.h>
#include<math.h>

#define ONE One[iter2]
#define TWO Two[iter2]

int main(int argc, char** argv)
{
	if( argc != 4 )
	{
		fprintf(stderr,"I can accept 4 arguments only.\n[N] file file\n");
		return 11;
	}

	register int N=atoi(argv[1]);
	register FILE *one,*two;
	one = fopen(argv[2],"rb");
	two = fopen(argv[3],"rb");
	
	if( one == NULL )
	{
		fprintf(stderr,"I can't find %s\n",argv[2]);
		return 11;
	}
	else if( two == NULL )
	{
		fprintf(stderr,"I can't find %s\n",argv[3]);
		return 11;
	}

	register int iter1,iter2;
	double One[N],Two[N];

	for( iter1=0 ; iter1<N ; iter1+=1 )
	{
		fread(One,sizeof(double),N,one);
		fread(Two,sizeof(double),N,two);
		for( iter2=0 ; iter2<N ; iter2+=1 )
		{
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
