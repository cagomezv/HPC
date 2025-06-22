int n=100, N=(n+1)*(n+1);
double h=1.0/double(n);
double 
  *u = new double[N],
  *unew = new double[N];
#define U(i,j) u((n+1)*(j)+(i))
#define UNEW(i,j) unew((n+1)*(j)+(i))

// Assign b.c. values
for (int i=0; i<=n; i++) {
  U(i,0) = g(h*i,0.0);
  U(i,n) = g(h*i,1.0);
}

for (int j=0; j<=n; j++) {
  U(0,j) = g(0.0,h*j);
  U(n,j) = g(1.0,h*j);
}

// Jacobi iteration
for (int j=1; j<n; j++) {
  for (int i=1; i<n; i++) {
    UNEW(i,j) = 0.25*(U(i-1,j)+U(i+1,j)+U(i,j-1)
		      +U(i,j+1)-h*h*F(i,j));
  }
}
