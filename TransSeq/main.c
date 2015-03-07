#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

FILE *input=NULL,*output=NULL;
double **A=NULL,**B = NULL,**C=NULL;
int N;

#include "../HeaderFiles/matrix_all_func_2DMat.h"

int main2()
{
	fprintf(stderr,"Not finished");
	return 11;
}

int main(int argc, char** argv)
{
//	return main2();

	if( argc!=2 )
	{
		fprintf(stderr,"Usage:./main N");
		return 11;
	}
	
	N = atoi(argv[1]);
//	N = 3;
	
	char input_file[60],output_file[20];
	
	sprintf(input_file,"../OctaveInputs/%i/input%i.bin",N,N);
	input = fopen(input_file,"rb");  // Although 'b' in ignored,

	if( input==NULL )
	{
		fprintf(stderr,"The input file \"input%i.bin\" does not exist.\n",N);
		return 11;
	}

	sprintf(output_file,"output%i.bin",N);
	output = fopen(output_file,"wb");// Adding it to remind ourselves that these are binary files
	
	fprintf(stderr,"Allocating %ix%i matrices\n",N,N);

	AllocB();
	
	GetBTrans();

	MultABTransSeqHIGHMEM();

	FreeB();

	fclose(input);
	fclose(output);

	return 0;
}
