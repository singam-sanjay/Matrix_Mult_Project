#include<time.h>
#include<omp.h>
#include<mkl.h>

#define NUMT 4
#define MATRIX_ALL_FUNC_1DMAT

#include"./GFlOps.h"

void AllocA()
{
	fprintf(stderr,"Allocating A.\n");
	while( A==NULL )
	{
		A = (double*)malloc( sizeof(double)*N*N );
	}
	fprintf(stderr,"Done Allocating A.\n");
}

void MKLAllocA()
{
	fprintf(stderr,"Allocating A.\n");
	while( A==NULL )
	{
		A = (double*)mkl_malloc( sizeof(double)*N*N ,64);
	}
	fprintf(stderr,"Done Allocating A.\n");
}

void AllocB()
{
	fprintf(stderr,"Allocating B.\n");
	while( B==NULL )
	{
		B = (double*)malloc( sizeof(double)*N*N );
	}
	fprintf(stderr,"Done Allocating B.\n");
}

void MKLAllocB()
{
	fprintf(stderr,"Allocating B.\n");
	while( B==NULL )
	{
		B = (double*)mkl_malloc( sizeof(double)*N*N ,64);
	}
	fprintf(stderr,"Done Allocating B.\n");
}

void AllocC()
{
	fprintf(stderr,"Allocating C.\n");
	while( C==NULL )
	{
		C = (double*)malloc( sizeof(double)*N*N );
	}
	fprintf(stderr,"Done Allocating C.\n");
}


void MKLAllocC()
{
	fprintf(stderr,"Allocating C.\n");
	while( C==NULL )
	{
		C = (double*)mkl_malloc( sizeof(double)*N*N ,64);
	}
	fprintf(stderr,"Done Allocating C.\n");
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
	fprintf(stderr,"The actual function is in development.\n");
}

void MultABTransParLEGACYOrig()
{
	/* Cache Aware Multiplication
	*
	* We keep a set of rows from matrix A in the cache, compute all possible results in C which involve the rows, and then move onto
	* the next set of rows in A and repeat the same.
	*
	* for coulumn in COULUMN(B): # N iterations
	* 	for row in L3_Cache: # "jump" iterations
	* 		result = row*coulumn; # N iterations
	*
	* Let's say there are "jump" rows in the cache. Hence this of O(N*jump*N) complexity. Since there N rows in matrix A and "jump" rows
	* are used in the cache, the rows have to be moved to the cache ceil(N/jump) times (ceil since N may not be a multiple of jump, therefor
	* the last r(=N%jump) rows also need to be involved in the multiplication, hence there are floor(N/jump)+1( if N not mult of jump) == floor(N/jump)
	* 
	* Total complexity = O(N/jump)*O(N*jump*N) = O(N*N*N)
	*
	* To maximise efficiency, maximum possible rows from A would be filled in L3_Cache ( Improves temporal locality, same column of B used repeatedly
	* ,each time with a different row of A in L3_Cache )
	*
	* max(jump) = sizeof(L3_Cache)/sizeof(Row)
	* 
	*/
	omp_set_num_threads(NUMT);
	#define sizeof_L3 (4096lu*1024lu) // 4096kB = 4096 * 1024 // next time it should be a function
	register size_t jump = sizeof_L3/(N*sizeof(double)); // SPACE/sizeof_1_row => ( MEM / (MEM/ROW) ) ==> ROW therefore #rows in L3 cache
	double start = omp_get_wtime(),time_taken;
	unsigned long long clk_cnt_start=__rdtsc(),clk_cnt_stop;

	#pragma omp parallel
	{
		register int iter1,iter2,iter3,last = (N*(omp_get_thread_num()+1))/NUMT;
		register double *a,*b,*c;
		for( iter1=(N*omp_get_thread_num())/NUMT ,a=A+iter1*N ,c=C+iter1*N ; iter1<last ; iter1+=jump,a+=(jump*N),c+=(jump*N) )
		{
			for(iter2=0,b=B ; iter2<N ; iter2+=1,b+=N)
			{
			
				bzero(c,sizeof(double)*jump);
				/*
				result1 = 0.0f;
				result2 = 0.0f;
				result3 = 0.0f;
				result4 = 0.0f;
				
				#pragma unroll
				for( iter3=0 ; iter3<_N ; iter3+=1 )
				{
					bval = *(b+iter3);
					result1 += (*(a+iter3      )) * bval;
					result2 += (*(a+iter3+  _N )) * bval;
					result3 += (*(a+iter3+ _2N )) * bval;
					result4 += (*(a+iter3+ _3N )) * bval;
				}
				
				*(c+iter2    ) = result1;
				*(c+iter2+ _N) = result2;
				*(c+iter2+_2N) = result3;
				*(c+iter2+_3N) = result4;
				*/
			}
		}
	}
	clk_cnt_stop = __rdtsc();
	printf("\n%zu : %lf GFlops:%lf\n",N,time_taken=(omp_get_wtime()-start),GFlops((clk_cnt_stop-clk_cnt_start),2.0l*N*N*N,get_cpu_freq_ghz()));
	fprintf(stderr,"\nTime Taken:%lf sec\n",time_taken);
	return;
}

void MKLMultAB()
{
	omp_set_num_threads(NUMT);
	double start = omp_get_wtime(),time_taken;
	unsigned long long clk_cnt_start=__rdtsc(),clk_cnt_stop;
	{
		#define alpha 1.0
		#define beta  0.0
		cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, alpha, A, N, B, N, beta, C, N);
	}
	clk_cnt_stop = __rdtsc();
	printf("\n%zu : %lf GFlops:%lf\n",N,time_taken=(omp_get_wtime()-start),GFlops((clk_cnt_stop-clk_cnt_start),2.0l*N*N*N,get_cpu_freq_ghz()));
	fprintf(stderr,"\nTime Taken:%lf sec\n",time_taken);
	return;
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

MKLFreeAll()
{
	fprintf(stderr,"Freeing A,B and C\n");
	mkl_free(A);
	mkl_free(B);
	mkl_free(C);
	fprintf(stderr,"Done Freeing A,B and C\n");
}
