#include<stdio.h>
#include<string.h>

int main(int argc, char** argv)
{
	if( argc!=2 )
	{
		fprintf(stderr,"Can take one argument only.\nUsage:$Read_binary_file output[N].bin\n");
		return 11;
	}

	register FILE *file = fopen(argv[1],"rb");

	if( file==NULL )
	{
		fprintf(stderr,"File ain't exist.\n");
		return 11;
	}
	
	int N;
	fprintf(stderr,"N:");
	scanf("%i",&N);

	double input[N];

	register int iter1,iter2;

	for( iter1=0 ; iter1<N ; iter1+=1 )
	{
		memset(input,0,sizeof(double)*N);
		fread(input,sizeof(double),N,file);
		printf("%.13lf",input[0]);
		for( iter2=1 ; iter2<N ; iter2+=1 )
		{
			printf(" %.13lf",input[iter2]);
		}
		putchar('\n');
	}

	return 0;
}
