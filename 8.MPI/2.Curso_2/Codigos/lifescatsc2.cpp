if (myrank % 2 == 0 ) {
  if (myrank<size-1) {
    SEND(i2-1,myrank+1);
    RECEIVE(I2-1,myrank+1);
  }
} else {
  if (myrank>0) {
    RECEIVE(I1,myrank-1);
    SEND(i1,myrank-1);
  }
}

if (myrank % 2 == 0 ) {
  if (myrank>0) {
    SEND(i1,myrank-1);
    RECEIVE(I1,myrank-1);
  }
} else {
  if (myrank<size-1) {
    RECEIVE(I2-1,myrank+1);
    SEND(i2-1,myrank+1);
  }
}
