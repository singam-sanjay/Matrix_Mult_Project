#include<stdio.h>
#include<stdlib.h>

int main(int argc, char** argv)
{
	if( argc!=1 )
	{
		fprintf(stderr,"I dont take any arguments.\nUsage:Make_Big_Binary_File\n");
		return 11;
	}

	int N;
	fprintf(stderr,"N:");
	scanf("%i",&N);

	char file_name[50];
	sprintf(file_name,"answer%i.bin",N);

	FILE *file = fopen(file_name,"wb");

	if( file==NULL )
	{
		fprintf(stderr,"I'm Sorry, File ain't opening.\n");
		return 11;
	}

	double *input = NULL;
	while( input==NULL )
		input=(double*)malloc( sizeof(double)*N );

	register int iter1,iter2;

	fprintf(stderr,"Enter C:\n");
	for( iter1=0 ; iter1<N ; iter1+=1 )
	{
		for( iter2=0 ; iter2<N ; iter2+=1 )
			scanf("%lf",input+iter2);
		fwrite(input,sizeof(double),N,file);
		fprintf(stderr,"\r%i/%i",iter1+1,N);
	}
	putc('\n',stderr);

	return 0;
}
