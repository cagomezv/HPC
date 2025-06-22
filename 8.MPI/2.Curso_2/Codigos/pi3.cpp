// $Id: pi3.cpp,v 1.1 2004/07/22 17:44:50 mstorti Exp $

//***********************************************************
//   pi3.cpp - compute pi by integrating f(x) = 4/(1 + x**2)
//
//   Each node:
//    1) receives the number of rectangles used 
//              in the approximation.
//    2) calculates the areas of it's rectangles.
//    3) Synchronizes for a global summation.
//   Node 0 prints the result.
//
//  Variables:
//
//    pi       the calculated result
//    n        number of points of integration.
//    x        midpoint of each rectangle's interval
//    f        function to integrate
//    sum,pi   area of rectangles
//    tmp      temporary scratch space for global summation
//    i        do loop index
//**********************************************************

#include <mpi.h>
#include <cstdio>
#include <cmath>

// The function to integrate
double f(double x) { return 4./(1.+x*x); }

int main(int argc, char **argv) {

  // Initialize MPI environment 
  MPI_Init(&argc,&argv);

  // Get the process number and assign it to the variable myrank
  int myrank;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

  //  Determine how many processes the program will run on and
  //  assign that number to size
  int size;
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  // The exact value 
  double PI=4*atan(1.0);

  // Enter an infinite loop. Will exit when user enters n=0
  while (1) {
    int n;
    //  Test to see if this is the program running on process 0,
    //  and run this section of the code for input.
    if (!myrank) {
      printf("Enter the number of intervals: (0 quits) > ");
      scanf("%d",&n);
    }

    //  The argument 0 in the 4th place indicates that
    //  process 0 will send the single integer n to every
    //  other process in processor group MPI_COMM_WORLD.
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);

    //  If the user puts in a negative number for n we leave
    //  the program by branching to MPI_FINALIZE
    if (n<0) break;

    //  Now this part of the code is running on every node
    //  and each one shares the same value of n.  But all
    //  other variables are local to each individual
    //  process. So each process then calculates the each
    //  interval size.

    //*****************************************************c
    //         Main Body : Runs on all processors
    //*****************************************************c
    // even step size h as a function of partions
    double h = 1.0/double(n);	
    double sum  = 0.0;
    for (int i=myrank+1; i<=n; i += size) {
      double x = h * (double(i) - 0.5);
      sum = sum + f(x);
    }
    double pi, mypi = h * sum; // this is the total area
				// in this process,
				// (a partial sum.)

    // Each individual sum should converge also to PI,
    // compute the max error
    double error, my_error = fabs(size*mypi-PI);
    MPI_Reduce(&my_error,&error,1,MPI_DOUBLE,
	       MPI_MAX,0,MPI_COMM_WORLD);

    //  After each partition of the integral is calculated
    //  we collect all the partial sums.  The MPI_SUM
    //  argument is the operation that adds all the values
    //  of mypi into pi of process 0 indicated by the 6th
    //  argument.
    MPI_Reduce(&mypi,&pi,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

    //*****************************************************
    //           Print results from Process 0
    //*****************************************************

    //  Finally the program tests if myrank is node 0
    //  so process 0 can print the answer.
    if (!myrank) printf("pi is aprox: %f, "
		      "(error %f,max err over procs %f)\n",
		      pi,fabs(pi-PI),my_error);
    //  Run the program again.
    //
  }
  // Branch for the end of program.  MPI_FINALIZE will close
  // all the processes in the active group.

  MPI_Finalize();
}
