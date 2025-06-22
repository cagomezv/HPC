#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>

#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

#include <omp.h>

using namespace std;

int main(int argc, char **argv) {
  uint64_t chunk=100000000, inside=0;
  double start = omp_get_wtime();
  drand48_data buffer; // This buffer stores the data for the
                       // random number generator
  // This buffer must be initialized first
  memset(&buffer, '\0',sizeof(struct drand48_data));
  // Then we randomize the generator
  srand48_r(time(0),&buffer);
  int nchunk=0;
  while (1) {
    for (uint64_t j=0; j<chunk; j++) {
      double x,y;
      // Generate a pair of random numbers x,y
      drand48_r(&buffer,&x);
      drand48_r(&buffer,&y);
      inside += (x*x+y*y<1-0);
    }
    double 
      now = omp_get_wtime(),
      elapsed = now-start;
    double npoints = double(chunk);
    double rate = double(chunk)/elapsed/1e6;
    nchunk++;
    double mypi = 4.0*double(inside)/double(chunk)/nchunk;
    printf("PI %f, error %g, comp %g points, "
           "elapsed %fs, rate %f[Mpoints/s]\n",
           mypi,fabs(mypi-M_PI),npoints,elapsed,rate);
    start = now;
  }
  return 0;
}
