#include<time.h>
#include<omp.h>
#include<mkl.h>

#define NUMT 4
#define MATRIX_ALL_FUNC_1DMAT

#define D(x) ((double)(x))
#define get_cpu_freq() (mkl_get_cpu_frequency()*1000000000.0)


static __inline__ unsigned long long __rdtsc(void)
{
	  unsigned hi, lo;
	    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	      return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}


inline double get_cpu_freq_ghz()
{
	unsigned long long t0, t1;
	t0 = __rdtsc();
	sleep(1);
	t1 = __rdtsc();
	return (D(t1-t0)/1e09);
}

#define GFlops(cycles, nflops, freqghz) (D(freqghz)*D(nflops)/D(cycles))

void AllocA()
{
	fprintf(stderr,"Allocating A.\n");
	while( A==NULL )
	{
		A = (double*)malloc( sizeof(double)*N*N );
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

void AllocC()
{
	fprintf(stderr,"Allocating C.\n");
	while( C==NULL )
	{
		C = (double*)malloc( sizeof(double)*N*N );
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
	omp_set_num_threads(NUMT);
	double start = omp_get_wtime(),time_taken;
	unsigned long long clk_cnt_start=__rdtsc(),clk_cnt_stop;

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
	clk_cnt_stop = __rdtsc();
	printf("\n%zu : %lf GFlops:%lf\n",N,time_taken=(omp_get_wtime()-start),GFlops((clk_cnt_stop-clk_cnt_start),2.0l*N*N*N,get_cpu_freq_ghz()));
	fprintf(stderr,"\nTime Taken:%lf sec\n",time_taken);

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
