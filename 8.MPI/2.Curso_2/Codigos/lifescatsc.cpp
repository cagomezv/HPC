if (myrank<size-1) SEND(row(i2-1),myrank+1);
if (myrank>0) RECEIVE(row(I1),myrank-1);

if (myrank>0) SEND(row(i1),myrank-1);
if (myrank<size-1) RECEIVE(row(I2),myrank+1);
