//$Id: primes1.cpp,v 1.2 2005/04/29 02:35:28 mstorti Exp $
#include <cstdio>
#include <cmath>

int is_prime(int n) {
  if (n<2) return 0;
  int m = int(sqrt(n));
  for (int j=2; j<=m; j++) 
    if (n % j ==0) return 0;
  return 1;
}

// Sequential version

int main(int argc, char **argv) {

  int n=2, primes=0,chunk=10000;
  while(1) {
    if (is_prime(n++)) primes++;
    if (!(n % chunk)) printf("%d primes<%d\n",primes,n);
  }
}
