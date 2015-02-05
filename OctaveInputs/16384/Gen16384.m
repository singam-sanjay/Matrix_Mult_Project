N = 16384;
c = rand(N,N)/((2**0.5)**4);
b = rand(N,N)/((2**0.5)**4);
N = [ N ];
input_file = sprintf('input%i.txt',N);
dlmwrite(input_file,N," ");
printf("Writing a\n");
dlmwrite(input_file,c," ","-append");
printf("Writing b\n");
dlmwrite(input_file,b," ","-append");

printf("Multiplying a and b\n");
c = c*b;
clear b;
output_file = sprintf('answer%i.txt',N);
dlmwrite(output_file,N," ");
printf("Writing c\n");
dlmwrite(output_file,c," ","-append");
