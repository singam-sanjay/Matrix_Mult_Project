#include<time.h>
#include<omp.h>

#define NUMT 4
#define MATRIX_ALL_FUNC_1DMAT



void AllocA()
{
	while( A==NULL )
	{
		A = (double*)malloc( sizeof(double)*N*N );
	}
}

void AllocB()
{
	while( B==NULL )
	{
		B = (double*)malloc( sizeof(double)*N*N );
	}
}

void AllocC()
{
	while( C==NULL )
	{
		C = (double*)malloc( sizeof(double)*N*N );
	}
}

void GetA()
{
	fread(A,sizeof(double),N*N,input);
}

void GetBSimple()
{
	fread(B,sizeof(double),N*N,input);
}

void GetBTrans()
{
	register int iter1,iter2;
	fseek(input,sizeof(double)*N*N,SEEK_SET);
	for( iter1 = 0; iter1<N ; iter1+=1 )
	{
		for( iter2=0 ; iter2<N ; iter2+=1 )
		{
			fread(B+iter2*N+iter1,sizeof(double),1,input);
		}
	}
}

void MultABTransParLEGACY()
{
	omp_set_num_threads(NUMT);
	double start = omp_get_wtime(),time_taken;

	#pragma omp parallel
	{
		register int iter1,iter2,iter3,last = (N*(omp_get_thread_num()+1))/NUMT;
		register double result,*a,*b,*c;
		//printf("%i\n",omp_get_thread_num());
		for( iter1=(N*omp_get_thread_num())/NUMT ,a=A+iter1*N ,c=C+iter1*N ; iter1<last ; iter1+=4,a+=(4*N),c+=(4*N) )
		{
			for(iter2=0,b=B ; iter2<N ; iter2+=1,b+=N)
			{
				result = 0.0f;
				for( iter3=0 ; iter3<N ; iter3+=1 )
				{
					result += (*(a+iter3)) * (*(b+iter3));
				}
				*(c+iter2) = result;

				result = 0.0f;
				a += N;
				for( iter3=0 ; iter3<N ; iter3+=1 )
				{
					result += (*(a+iter3)) * (*(b+iter3));
				}
				*(c+N+iter2) = result;

				result = 0.0f;
				a += N;
				for( iter3=0 ; iter3<N ; iter3+=1 )
				{
					result += (*(a+iter3)) * (*(b+iter3));
				}
				*(c+(N<<1)+iter2) = result;

				result = 0.0f;
				a += N;
				for( iter3=0 ; iter3<N ; iter3+=1 )
				{
					result += (*(a+iter3)) * (*(b+iter3));
				}
				*(c+(N<<1)+N+iter2) = result;
				a -= (3*N);

			}
		}
	}
	fprintf(stderr,"\nTime Taken:%lf sec\n",time_taken=(omp_get_wtime()-start));
	printf("\n%zu : %lf",N,time_taken);
}

PutC()
{
	fwrite(C,sizeof(double),N*N,output);
}

FreeAll()
{
	fprintf(stderr,"Freeing A,B and C\n");
	free(A);
	free(B);
	free(C);
	fprintf(stderr,"Done Freeing A,B and C\n");
}
