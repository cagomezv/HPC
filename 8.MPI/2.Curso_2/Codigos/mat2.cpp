// Internal storage is released in destructor
Mat A(100,100); 
// usar `A' ...
  
// Internal storage must be released by the user
double *b = new double[10000];
Mat B(100,100,b);
// use `B' ...
delete[] b;
