#include<time.h>
#include<omp.h>

#define MATRIX_ALL_FUNC_2DMAT
#define NUMT 4

#include"./GFlOps.h"

//#define DEBUG
//#define TOOMUCHTODEBUG

void AllocA()
{
	#ifdef DEBUG
	fprintf(stderr,"Allocating A\n");
	#endif
	while( A == NULL )
	{
		A = (double**)malloc( sizeof(double*)*N );
	}
	
	register int iter;
	for( iter=0 ; iter<N ; iter+=1 )
	{
		A[iter] = NULL;
		while( A[iter] == NULL )
		{
			A[iter] = (double*)malloc( sizeof(double)*N );
		}
	}
	#ifdef DEBUG
	fprintf(stderr,"Done Allocating A\n");
	#endif	
	return;
}

void AllocB()
{
	#ifdef DEBUG
	fprintf(stderr,"Allocating B\n");
	#endif 
	while( B == NULL )
	{
		B = (double**)malloc( sizeof(double*)*N );
	}
	
	register int iter;
	for( iter=0 ; iter<N ; iter+=1 )
	{
		B[iter] = NULL;
		while( B[iter] == NULL )
		{
			B[iter] = (double*)malloc( sizeof(double)*N );
		}
	}
	#ifdef DEBUG
	fprintf(stderr,"Done Allocating B\n");
	#endif 
	return;
}

void AllocC()
{
	#ifdef DEBUG
	fprintf(stderr,"Allocating C\n");
	#endif 
	while( C == NULL )
	{
		C = (double**)malloc( sizeof(double*)*N );
	}
	
	register int iter;
	for( iter=0 ; iter<N ; iter+=1 )
	{
		C[iter] = NULL;
		while( C[iter] == NULL )
		{
			C[iter] = (double*)malloc( sizeof(double)*N );
		}
	}
	#ifdef DEBUG
	fprintf(stderr,"Done Allocating C\n");
	#endif 
	return;
}

void GetA()
{
	register int iter1;
	#ifdef DEBUG
	fprintf(stderr,"Getting A\n");
	#endif 
	
	for( iter1 = 0; iter1<N ; iter1+=1 )
	{
		fread(A[iter1],sizeof(double),N,input);
	}
	#ifdef DEBUG
	fprintf(stderr,"Done getting A\n");
	#endif 
}

void GetBSimple()
{
	register int iter1;
	#ifdef DEBUG
	fprintf(stderr,"Getting B Simple\n");
	#endif
	for( iter1 = 0; iter1<N ; iter1+=1 )
	{
		fread(B[iter1],sizeof(double),N,input);
	}
	#ifdef DEBUG
	fprintf(stderr,"Done getting B Simple\n");
	#endif
}

void GetBTrans()
{
	register int iter1,iter2;
	#ifdef DEBUG
	fprintf(stderr,"Getting B Transpose\n");
	#endif
	fseek(input,sizeof(double)*N*N,SEEK_SET);
	for( iter1 = 0; iter1<N ; iter1+=1 )
	{
		for( iter2=0 ; iter2<N ; iter2+=1 )
		{
			fread(B[iter2]+iter1,sizeof(double),1,input);
		}
	}
	#ifdef DEBUG
	fprintf(stderr,"Done getting B Transpose\n");
	#endif
}

void MultABSimpleSeqHIGHMEM()
{
	register int iter1,iter2,iter3;
	double A[N],C[N],time_taken=0.0f;
	register clock_t start;
	//register double *a = A,*c = C;
	register double result;

	fseek(input,0,SEEK_SET);

	for( iter1=0 ; iter1<N ; iter1+=1/*,a+=1,c+=1*/ )
	{
		fprintf(stderr,"\r%i/%i",iter1+1,N);
		fread(A,sizeof(double),N,input);
		start = clock();
		for(iter2=0 ; iter2<N ; iter2+=1 )
		{
			result = 0.0f;
		//	C[iter2] = 0.0f;
			for( iter3=0 ; iter3<N ; iter3+=1 )
			{
			//	result += *(*(a)+iter3) * *(*(B+iter3)+iter2) ;
				result += A[iter3]*B[iter3][iter2];
			}
		//	*(*(c)+iter2) = result;
			C[iter2] = result;
		}
		time_taken += ((double)(clock()-start))/CLOCKS_PER_SEC ;
		fwrite(C,sizeof(double),N,output);
	}
//	fprintf(stderr,"\ntime taken:%lf sec\n",time_taken);
	printf("\n%i : %lf",N,time_taken);
}

void MultABSimpleParLEGACY()
{
	omp_set_num_threads(NUMT);
	double start = omp_get_wtime();

	#pragma omp parallel
	{
		register int iter1,iter2,iter3,last = (N*(omp_get_thread_num()+1))/NUMT;
		register double result,**c,**a;
		fprintf(stderr,"%i\n",omp_get_thread_num());
		for( iter1=(N*omp_get_thread_num())/NUMT ,a=A+iter1 ,c=C+iter1 ; iter1<last ; iter1+=1,a+=1,c+=1 )
		{
			for(iter2=0 ; iter2<N ; iter2+=1)
			{
				result = 0.0f;
				for( iter3=0 ; iter3<N ; iter3+=1 )
				{
					result += (*(*(a)+iter3)) * (*(*(B+iter3)+iter2));
					//C[iter1][iter2] += A[iter1][iter3]*A[iter3][iter2];
				}
				*(*(c)+iter2) = result;
			}
		}
	}
	fprintf(stderr,"\nTime Taken:%lf sec\n",omp_get_wtime()-start);
	printf("%i:%lf sec\n",N,omp_get_wtime()-start);
}

void MultABTransSeqHIGHMEM()
{
	register int iter1,iter2,iter3;
	register double time_taken=0.0f,**b;
	register double start;
	register double result;
	double A[N],C[N];
	unsigned long long clk_cnt_start,clk_cnt_stop;

	fseek(input,0,SEEK_SET);
	start = omp_get_wtime();
	clk_cnt_start = __rdtsc();
	for( iter1=0 ; iter1<N ; iter1+=1 )
	{
		#ifdef TOOMUCHTODEBUG
		fprintf(stderr,"\r%i/%i",iter1+1,N);
		#endif
		fread(A,sizeof(double),N,input);
		for(iter2=0,b=B ; iter2<N ; iter2+=1,b+=1 )
		{
			result = 0.0f;
			for( iter3=0 ; iter3<N ; iter3+=1 )
			{
				result += A[iter3] * *(*(b)+iter3);
			}
			C[iter2] = result;
		}
		fwrite(C,sizeof(double),N,output);
	}
	clk_cnt_stop = __rdtsc();
	printf("\n%zu : %lf GFlops:%lf\n",N,time_taken=(omp_get_wtime()-start),GFlops((clk_cnt_stop-clk_cnt_start),2.0l*N*N*N,get_cpu_freq_ghz()));
	fprintf(stderr,"\nTime Taken:%lf sec\n",time_taken);
}

void MultABTransParLEGACY()
{
	omp_set_num_threads(NUMT);
	double start = omp_get_wtime(),time_taken;
	unsigned long long clk_cnt_start=__rdtsc(),clk_cnt_stop;

	#pragma omp parallel
	{
		register int iter1,iter2,iter3,last = (N*(omp_get_thread_num()+1))/NUMT;
		register double result,**a,**b,**c;
		//printf("%i\n",omp_get_thread_num());
		for( iter1=(N*omp_get_thread_num())/NUMT ,a=A+iter1 ,c=C+iter1 ; iter1<last ; iter1+=1,a+=1,c+=1 )
		{
			for(iter2=0,b=B ; iter2<N ; iter2+=1,b+=1)
			{
				result = 0.0f;
				for( iter3=0 ; iter3<N ; iter3+=1 )
				{
					result += (*(*(a)+iter3)) * (*(*(b)+iter3));
					//C[iter1][iter2] += A[iter1][iter3]*B[iter2][iter3];
				}
				*(*(c)+iter2) = result;
			}
		}
	}
	clk_cnt_stop = __rdtsc();
	printf("\n%zu : %lf GFlops:%lf\n",N,time_taken=(omp_get_wtime()-start),GFlops((clk_cnt_stop-clk_cnt_start),2.0l*N*N*N,get_cpu_freq_ghz()));
	fprintf(stderr,"\nTime Taken:%lf sec\n",time_taken);
}

void PutC()
{
	register int iter1;
	#ifdef DEBUG
	fprintf(stderr,"Writing C.\n");
	#endif
	for( iter1=0 ; iter1<N ; iter1+=1 )
	{
		fwrite(C[iter1],sizeof(double),N,output);
	}
	#ifdef DEBUG
	fprintf(stderr,"Done writing C.\n");
	#endif
}

void FreeAll()
{
	register int iter;
	#ifdef DEBUG
	fprintf(stderr,"Freeing A,B and C\n");
	#endif
	for( iter=0 ; iter<N ; iter += 1)
	{
		free(A[iter]);free(B[iter]);free(C[iter]);
	}

	free(A);free(B);free(C);
	#ifdef DEBUG
	fprintf(stderr,"Done Freeing A,B and C\n");
	#endif
	return;
}

void FreeB()
{
	register int iter;
	#ifdef DEBUG
	fprintf(stderr,"Freeing B\n");
	#endif
	for( iter=0 ; iter<N ; iter += 1)
	{
		free(B[iter]);
	}

	free(B);
	#ifdef DEBUG
	fprintf(stderr,"Done Freeing B\n");
	#endif
	return;
}
