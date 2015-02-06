#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

FILE *input=NULL,*output=NULL;
double *A = NULL,*B = NULL,*C = NULL;
size_t N;

#include "../HeaderFiles/matrix_all_func_1DMat.h"

int main(int argc, char** argv)
{
	if( argc!=2 )
	{
		fprintf(stderr,"Usage:./main N\n");
		return 11;
	}

	N = strtoull(argv[1],NULL,10);
//	N = 3;

	char input_file[50],output_file[20];

	sprintf(input_file,"../OctaveInputs/%zu/input%zu.bin",N,N);
	input = fopen(input_file,"rb");  // Although 'b' in ignored,

	if( input==NULL )
	{
		fprintf(stderr,"The input file \"input%zu.bin\" does not exist.\n",N);
		return 11;
	}

	sprintf(output_file,"output%zu.bin",N);
	output = fopen(output_file,"wb");// Adding it to remind ourselves that these are binary files

	fprintf(stderr,"Allocating 3 %zux%zu matrices\n",N,N);

	AllocA();
	AllocB();
	AllocC();

	GetA();
	GetBTrans();

	MultABTransParLEGACY();

	PutC();

	FreeAll();

	fclose(input);
	fclose(output);

	return 0;
}
