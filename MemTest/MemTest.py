#Use Python3

def MemUsage(N,sizeofele):
    Mem = (N*sizeofele)**2;

    kB = Mem/1024.0;
    mB = kB/1024.0;
    gB = mB/1024.0;

    print("kB = %f,mB = %f,gB = %f" % (kB,mB,gB) );
