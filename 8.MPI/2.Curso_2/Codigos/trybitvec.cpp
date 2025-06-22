#include <cstdio>

#define NBITCHAR 8 //L-code-b
class bit_vector_t { 
public:
  int nchar, N;
  unsigned char *buff;
  bit_vector_t(int N) 
  : nchar(N/NBITCHAR+(N%NBITCHAR>0)) {
    buff = new unsigned char[nchar];
    for (int j=0; j<nchar; j++) buff[j] = 0;
  }
  int get(int j) {
    return (buff[j/NBITCHAR] & (1<<j%NBITCHAR))>0;
  }
  void set(int j,int val) {
    if (val) buff[j/NBITCHAR] |= (1<<j%NBITCHAR);
    else buff[j/NBITCHAR] &= ~(1<<j%NBITCHAR);
  }
};          
//L-code-e

int main() {
  int N = 35;
  bit_vector_t v(N);
  for (int j=0; j<N; j++) v.set(j,j%3);
  for (int j=0; j<N; j++) 
    printf("%d -> %d\n",j,v.get(j));
}
  
