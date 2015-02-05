a = rand(1023,1023);
b = rand(1023,1023);
c = a*b;

N = [ 1023 ];

dlmwrite("input1023.txt",N," ");
dlmwrite("input1023.txt",a," ","-append");
dlmwrite("input1023.txt",b," ","-append");
dlmwrite("answer1023.txt",c," ");


