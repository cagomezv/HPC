// define s,e ...
int rows_here = e-s+2;
double 
  *u = new double[(n+1)*rows_here],
  *unew = new double[(n+1)*rows_here];
#define U(i,j) u((n+1)*(j-s+1)+i)
#define UNEW(i,j) unew((n+1)*(j-s+1)+i)

// Assign b.c. values ...

// Jacobi iteration
for (int j=s; j<e; j++) {
  for (int i=1; i<n; i++) {
    UNEW(i,j) = 0.25*(U(i-1,j)+U(i+1,j)+U(i,j-1)
		      +U(i,j+1)-h*h*F(i,j));
  }
}
