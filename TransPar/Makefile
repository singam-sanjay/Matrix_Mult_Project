main.c : ../HeaderFiles/matrix_all_func_2DMat.h ../HeaderFiles/GFlOps.h 
	touch main.c

i_prog : main.c
	icc main.c -fopenmp -mkl -Wall -o i_prog

i_prog-g : main.c
	icc main.c -fopenmp -mkl -Wall -g -o i_prog-g

