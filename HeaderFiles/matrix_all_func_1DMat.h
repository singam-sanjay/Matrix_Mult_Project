#include<time.h>
#include<omp.h>
#include<mkl.h>

#define MATRIX_ALL_FUNC_1DMAT

#include"./GFlOps.h"

//#define TOOMUCHTODEBUG
#define DEBUGNUMT 4

#ifdef TOOMUCHTODEBUG
	#define NUMT DEBUGNUMT
#else
	#define NUMT 4
#endif


void inline nsleep()
{
	static struct timespec hold = {0, 10000};
	nanosleep(&hold,NULL);
}

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

void MultABTransParLEGACYJFJ()
{
	fprintf(stderr,"The actual function is in development.\n");
}

void MultABTransParCacheAware()
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
	#define sizeof_L3 (4096*1024) // 4096kB = 4096 * 1024 // XX Replace by func XX
	register int jump = sizeof_L3/(N*sizeof(double)); // Is the number of rows that can be stored at a time in L3 cache
	double start = omp_get_wtime(),time_taken;
	unsigned long long clk_cnt_start=__rdtsc(),clk_cnt_stop;

	jump /= (NUMT); // We want each thread to store some rows in L3 cache

	#pragma omp parallel
	{
		register int iter1,iter2,iter3,iter4,last = (N*(omp_get_thread_num()+1))/NUMT;
		register double *a,*b,*c,result;
		#ifdef TOOMUCHTODEBUG
			fprintf(stderr,"Thread %i %i-%i rows\n",omp_get_thread_num(),(N*omp_get_thread_num())/NUMT,last-1);
			#pragma omp barrier
		#endif
		for( iter1=(N*omp_get_thread_num())/NUMT ,a=A+iter1*N ,c=C+iter1*N ; iter1<last ; iter1+=jump,a+=(jump*N),c+=(jump*N) )
		{
			#ifdef TOOMUCHTODEBUG
				#pragma omp barrier
				{ fprintf(stderr,"\nMultiplying %i-%i Rows of A with B @ thread %i\n",iter1,iter1+jump,omp_get_thread_num()); }
				#pragma omp barrier
			#endif
			for(iter2=0,b=B ; iter2<N ; iter2+=1,b+=N)// iterate through the coulumns of B ( rows of B' in this case )
			{
				for( iter3=0; iter3<jump ; iter3+=1 )// iterate through rows of A and C
				{
					result = 0.0f;
					#pragma unroll
					for( iter4=0 ; iter4<N ; iter4+=1 )// iterate whithin the rows of A and columns of B ( rows of B' )
					{
						#ifdef TOOMUCHTODEBUG
							#pragma omp critical
							{ fprintf(stderr,"C[%i][%i] += A[%i][%i]*B[%i][%i]@T%i\r",iter3,iter2,iter3,iter4,iter4,iter2,omp_get_thread_num());nsleep(); }
						#endif
						result += ( *(a+iter4) * *(b+iter4) );
					}
					*(c+iter3*N+iter2) = result;
					a += N;
				}
				a -= (jump*N);

			}
		}
	}
	clk_cnt_stop = __rdtsc();
	printf("\n%zu : %lf GFlops:%lf\n",N,time_taken=(omp_get_wtime()-start),GFlops((clk_cnt_stop-clk_cnt_start),2.0l*N*N*N,get_cpu_freq_ghz()));
	fprintf(stderr,"\nTime Taken:%lf sec\n",time_taken);
	return;
}

double * MultABTransParCacheAware_ManyinAwithOneinB()
{
	static double retVAL[2];
	register int hor_tile_selector,ver_tile_selector,hor_tile_width,ver_tile_width;
	register const int _N = N;
	register double * b;

	#define _1KB 	      (1024)
	#define L3_cache_size (4096*_1KB)
	#define L2_cache_size (256 *_1KB)
	
	#define hor_tile_length N
	#define ver_tile_length N

	#define A(r,c) (*(A + (r)*N + (c)   ))
	#define B(r,c) (*(B + (r)   + (c)*N )) // since B is in transpose
	#define C(r,c) (*(C + (r)*N + (c)   ))

	hor_tile_width = L3_cache_size/(_N*sizeof(double));
	ver_tile_width = 1;

	if( hor_tile_width>_N )
		hor_tile_width = _N;

	#ifdef _OPENMP
		double start = omp_get_wtime(), time;
		omp_set_num_threads(1);
	#else
		clock_t start = clock();
	#endif
	unsigned long long t1,t0 = __rdtsc();
	for( hor_tile_selector=0 ; hor_tile_selector<_N ; hor_tile_selector += hor_tile_width )
	{
		b = B;
		for( ver_tile_selector = 0 ; ver_tile_selector<_N ; ver_tile_selector+= ver_tile_width )
		{
			register int row;
			#pragma omp parallel for
			for( row = hor_tile_selector ; row<(hor_tile_selector+hor_tile_width) ; row+=1 )
			{
				register int iter;
				register double result = 0;
				register const double * a = A + row*N;
				#pragma unroll
				for( iter = 0 ; iter<_N ; iter+=1 )
				{
					#ifdef DEBUG
					//	printf("result += A[%i][%i] * B[%i][%i]\n",row,iter,iter,ver_tile_selector);
						printf("@T%i result += %lf * %lf\n",omp_get_thread_num(),a[iter],b[iter]);
					#endif
				//	result += A(row,iter) * B(iter,ver_tile_selector) ;
					result += a[iter] * b[iter];
				}
				#ifdef DEBUGSMALL
					printf("@T%i C[%i][%i] = %lf\n",omp_get_thread_num(),row,ver_tile_selector,result);
				#endif
				C(row,ver_tile_selector) = result;
			}
			b += _N;
		}
	}
	
	t1 = __rdtsc();
	#ifdef _OPENMP
		retVAL[0] = omp_get_wtime()-start; 
		retVAL[1] =GFlops( (t1-t0), (2.0f*N*N*N), get_cpu_freq_ghz()); 
		printf("Total time %i : %lfs GFlOps : %lf\n", _N, retVAL[0], retVAL[1]);
	#else
		retVAL[0] = ( (double)(clock()-start) )/CLOCKS_PER_SEC;
		retVAL[1] =GFlops( (t1-t0), (2.0f*N*N*N), get_cpu_freq_ghz()); 
		printf("Total time %i : %lfs GFlOps : %lf\n", _N, retVAL[0], retVAL[1]);
	#endif
	
	#undef A
	#undef B
	#undef C

	#undef _1KB
	#undef L3_cache_size
	#undef L2_cache_size

	return retVAL;
}

double *  MultABTransParCacheAware_ManyinAwithManyinB()
{
	static double retVAL[2];
	register int hor_tile_selector,ver_tile_selector,hor_tile_width,ver_tile_width;
	register const int _N = N;

	#define _1KB 	      (1024)
	#define L3_cache_size (4096*_1KB)
	#define L2_cache_size (256 *_1KB)
	
	#define hor_tile_length N
	#define ver_tile_length N

	#define A(r,c) (*(A + (r)*N + (c)   ))
	#define B(r,c) (*(B + (r)   + (c)*N )) // since B is in transpose
	#define C(r,c) (*(C + (r)*N + (c)   ))

	hor_tile_width = L3_cache_size/(_N*sizeof(double));
	ver_tile_width = L2_cache_size/(_N*sizeof(double));

	if( hor_tile_width>_N )
		hor_tile_width = _N;

	if( ver_tile_width>_N )
		ver_tile_width = _N;
	
	#ifdef _OPENMP
		double start = omp_get_wtime(), time;
		omp_set_num_threads(NUMT);
	#else
		clock_t start = clock();
	#endif
	unsigned long long t1,t0 = __rdtsc();
	for( hor_tile_selector=0 ; hor_tile_selector<_N ; hor_tile_selector += hor_tile_width )
	{
		for( ver_tile_selector = 0 ; ver_tile_selector<_N ; ver_tile_selector+= ver_tile_width )
		{
			register int col;
			#pragma omp parallel for
			for( col = ver_tile_selector ; col<(ver_tile_selector+ver_tile_width) ; col+=1 )
			{
				register int row,iter;
				register double result;
				register double * a = A+hor_tile_selector*N;
				register double * b = B+col*_N;
				for( row = hor_tile_selector ; row<(hor_tile_selector+hor_tile_width) ; row+=1 )
				{
					result = 0;
					for( iter = 0 ; iter<N ; iter+=1 )
					{
						#ifdef DEBUG
						//	printf("result += A[%i][%i] * B[%i][%i]\n",row,iter,iter,ver_tile_selector);
							printf("result += %lf * %lf\n",A(row,iter),B(iter,col));
						#endif
					//	result += A(row,iter) * B(iter,col) ;
						result += a[iter] * b[iter];
					}
					#ifdef DEBUGSMALL
						printf("C[%i][%i] = %lf\n",row,col,result);
					#endif
					C(row,col) = result;
					a += _N;
				}
			}
		}
	}
	
	t1 = __rdtsc();
	#ifdef _OPENMP
		retVAL[0] = omp_get_wtime()-start; 
		retVAL[1] =GFlops( (t1-t0), (2.0f*N*N*N), get_cpu_freq_ghz()); 
		printf("Total time %i : %lfs GFlOps : %lf\n", _N, retVAL[0], retVAL[1]);
	#else
		retVAL[0] = ( (double)(clock()-start) )/CLOCKS_PER_SEC;
		retVAL[1] =GFlops( (t1-t0), (2.0f*N*N*N), get_cpu_freq_ghz()); 
		printf("Total time %i : %lfs GFlOps : %lf\n", _N, retVAL[0], retVAL[1]);
	#endif
	
	#undef A
	#undef B
	#undef C

	#undef _1KB
	#undef L3_cache_size
	#undef L2_cache_size

	return retVAL;
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
