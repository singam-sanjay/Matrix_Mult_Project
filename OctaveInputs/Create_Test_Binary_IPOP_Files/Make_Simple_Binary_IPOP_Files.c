#include<stdio.h>

#define fpf fprintf
#define sd stderr

int main()
{
	int N;
	char file_name[50];
	fprintf(stderr,"N:");
	scanf("%i",&N);

	fpf(sd,"FileName[- for input$N.bin]:");
	scanf("%s",file_name);

	if( file_name[0]=='-' )
		sprintf(file_name,"input%i.bin",N);

	FILE *file = fopen(file_name,"wb");

	if( file==NULL )
	{
		fpf(sd,"I'm Sorry\n");
		return 11;
	}

	register int iter1,iter2;
	double A[N][N],B[N][N],C[N][N];

	fpf(sd,"Enter A:\n");

	for( iter1=0 ; iter1<N ; iter1+=1 )
	{
		fpf(sd,"Row %i:",iter1+1);
		for( iter2=0 ; iter2<N ; iter2+=1 )
		{
			scanf("%lf",&A[iter1][iter2]);
		}
	}
	fwrite(A,sizeof(double),N*N,file);
	
	fpf(sd,"Enter B:\n");

	for( iter1=0 ; iter1<N ; iter1+=1 )
	{
		fpf(sd,"Row %i:",iter1+1);
		for( iter2=0 ; iter2<N ; iter2+=1 )
		{
			scanf("%lf",&B[iter1][iter2]);
		}
	}
	fwrite(B,sizeof(double),N*N,file);

	register int iter3;
	for( iter1=0 ; iter1<N ; iter1+=1 )
	{
		for( iter2=0 ; iter2<N ; iter2+=1 )
		{
			C[iter1][iter2] = 0.0f;
			for( iter3=0 ; iter3<N ; iter3+=1 )
			{
				C[iter1][iter2] += A[iter1][iter3]*B[iter3][iter2];
			}
		}
	}

	fclose(file);
	sprintf(file_name,"answer%i.bin",N);
	file = fopen(file_name,"wb");
	fwrite(C,sizeof(double),N*N,file);

	return 0;
}
