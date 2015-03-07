
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
	static char called = 0;
	static double GHz;
	if( called )
	{
		return GHz;
	}
	else
	{
		unsigned long long t0, t1;
		t0 = __rdtsc();
		sleep(1);
		t1 = __rdtsc();
		called = 1;
		return (GHz =  (D(t1-t0)/1e09) );
	}
}


#define GFlops(cycles, nflops, freqghz) (D(freqghz)*D(nflops)/D(cycles))

