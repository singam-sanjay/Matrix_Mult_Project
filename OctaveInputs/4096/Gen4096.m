N = 4096;
a = rand(N,N)/(2);
b = rand(N,N)/(2);
c = a*b;

N = [ N ];

input_file = sprintf('input%i.txt',N);
output_file = sprintf('answer%i.txt',N);

dlmwrite(input_file,N," ");
dlmwrite(input_file,a," ","-append");
dlmwrite(input_file,b," ","-append");
dlmwrite(output_file,N," ");
dlmwrite(output_file,c," ","-append");


