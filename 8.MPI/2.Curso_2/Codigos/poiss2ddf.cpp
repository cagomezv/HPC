//$Id: poisson2D-df.c,v 1.1 2008/06/26 17:10:31 rodrigop Exp rodrigop $
// Program usage:  mpirun -np <procs> poisson.bin [-help] [all PETSc options]

static char help[] = "Solves a linear system in parallel with KSP.\n\
Input parameters include:\n\
  -random_exact_sol : use a random exact solution vector\n\
  -view_exact_sol   : write exact solution vector to stdout\n\
  -m <mesh_x>       : number of mesh points in x-direction\n\
  -n <mesh_n>       : number of mesh points in y-direction\n\n";

//#include "petscksp.h"
#include "petsc.h"
#include "petscsys.h"

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **args)
{
  Vec            x,b,u;    // approx solution, RHS, exact solution
  Mat            A;        // linear system matrix 
  KSP            ksp;      // linear solver context
  PetscRandom    rctx;     // random number generator context
  PetscReal      norm;     // norm of solution error
  PetscInt       i,j,I,J,Istart,Iend,n = 100,m,its;
  PetscErrorCode ierr;
  PetscBool     flg;
  PetscScalar    v,one = 1.0,neg_one = -1.0;

  PetscInitialize(&argc,&args,(char *)0,help);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-n",&n,PETSC_NULL);CHKERRQ(ierr);
  int n1 = n-1;
  m=n1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         Compute the matrix and right-hand-side vector that define
         the linear system, Ax = b.
     - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* 
     Create parallel matrix, specifying only its global dimensions.
     When using MatCreate(), the matrix format can be specified at
     runtime. Also, the parallel partitioning of the matrix is
     determined by PETSc at runtime.

     Performance tuning note:  For problems of substantial size,
     preallocation of matrix memory is crucial for attaining good 
     performance.  Since preallocation is not possible via the generic
     matrix creation routine MatCreate(), we recommend for practical 
     problems instead to use the creation routine for a particular matrix
     format, e.g.,
         MatCreateMPIAIJ() - parallel AIJ (compressed sparse row)
         MatCreateMPIBAIJ() - parallel block AIJ
     See the matrix chapter of the users manual for details.
  */
  ierr = MatCreate(PETSC_COMM_WORLD,&A);CHKERRQ(ierr);
  ierr = MatSetSizes(A,PETSC_DECIDE,PETSC_DECIDE,m*n1,m*n1);CHKERRQ(ierr);
  ierr = MatSetFromOptions(A);CHKERRQ(ierr);

  /* 
     Currently, all PETSc parallel matrix formats are partitioned by
     contiguous chunks of rows across the processors.  Determine which
     rows of the matrix are locally owned. 
  */
  ierr = MatGetOwnershipRange(A,&Istart,&Iend);CHKERRQ(ierr);

  /* 
     Set matrix elements for the 2-D, five-point stencil in parallel.
      - Each processor needs to insert only elements that it owns
        locally (but any non-local elements will be sent to the
        appropriate processor during matrix assembly). 
      - Always specify global rows and columns of matrix entries.

     Note: this uses the less common natural ordering that orders first
     all the unknowns for x = h then for x = 2h etc; Hence you see J = I +- n1
     instead of J = I +- m as you might expect. The more standard ordering
     would first do all variables for y = h, then y = 2h etc.
   */
  double h=1.0/(n1+1),
    kond=1.0,
    ax = 0.0,
    ay = 0.0,
    c = 0,
    coef = kond/(h*h),
    cO = 4*coef+c,
    cW = -coef-ax/(2*h),
    cE = -coef+ax/(2*h),
    cS = -coef-ay/(2*h),
    cN = -coef+ay/(2*h);
  for (I=Istart; I<Iend; I++) { 
    i = I/n1; j = I - i*n1;
    double x=i*h, y=j*h;
    if (i>0)   {J = I - n1; ierr =
            MatSetValues(A,1,&I,1,&J,&cS,INSERT_VALUES);CHKERRQ(ierr);}
    if (i<m-1) {J = I + n1; ierr =
            MatSetValues(A,1,&I,1,&J,&cN,INSERT_VALUES);CHKERRQ(ierr);}
    if (j>0)   {J = I - 1; ierr =
            MatSetValues(A,1,&I,1,&J,&cW,INSERT_VALUES);CHKERRQ(ierr);}
    if (j<n1-1) {J = I + 1; ierr =
            MatSetValues(A,1,&I,1,&J,&cE,INSERT_VALUES);CHKERRQ(ierr);}
    ierr = MatSetValues(A,1,&I,1,&I,&cO,INSERT_VALUES);CHKERRQ(ierr);
  }

  /* 
     Assemble matrix, using the 2-step process:
       MatAssemblyBegin(), MatAssemblyEnd()
     Computations can be done while messages are in transition
     by placing code between these two statements.
  */
  ierr = MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);

  /* 
     Create parallel vectors.
      - We form 1 vector from scratch and then duplicate as needed.
      - When using VecCreate(), VecSetSizes and VecSetFromOptions()
        in this example, we specify only the
        vector's global dimension; the parallel partitioning is determined
        at runtime. 
      - When solving a linear system, the vectors and matrices MUST
        be partitioned accordingly.  PETSc automatically generates
        appropriately partitioned matrices and vectors when MatCreate()
        and VecCreate() are used with the same communicator.  
      - The user can alternatively specify the local vector and matrix
        dimensions when more sophisticated partitioning is needed
        (replacing the PETSC_DECIDE argument in the VecSetSizes() statement
        below).
  */
  ierr = VecCreate(PETSC_COMM_WORLD,&u);CHKERRQ(ierr);
  ierr = VecSetSizes(u,PETSC_DECIDE,m*n1);CHKERRQ(ierr);
  ierr = VecSetFromOptions(u);CHKERRQ(ierr);
  ierr = VecDuplicate(u,&b);CHKERRQ(ierr); 
  ierr = VecDuplicate(b,&x);CHKERRQ(ierr);

  for (I=Istart; I<Iend; I++) { 
    i = I/n1; j = I - i*n1;
    double x=i*h, y=j*h;
#define SQ(x) (x)*(x)
    double r = sqrt(SQ(x-0.5)+SQ(y-0.5));
    double q = r>0.2 && r<0.25;
    VecSetValue(b,I,q,ADD_VALUES);
  }
  ierr = VecAssemblyBegin(b); CHKERRQ(ierr);
  ierr = VecAssemblyEnd(b); CHKERRQ(ierr);
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
                Create the linear solver and set various options
     - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* 
     Create linear solver context
  */
  ierr = KSPCreate(PETSC_COMM_WORLD,&ksp);CHKERRQ(ierr);

  /* 
     Set operators. Here the matrix that defines the linear system
     also serves as the preconditioning matrix.
  */
  ierr = KSPSetOperators(ksp,A,A,DIFFERENT_NONZERO_PATTERN);CHKERRQ(ierr);

  /* 
     Set linear solver defaults for this problem (optional).
     - By extracting the KSP and PC contexts from the KSP context,
       we can then directly call any KSP and PC routines to set
       various options.
     - The following two statements are optional; all of these
       parameters could alternatively be specified at runtime via
       KSPSetFromOptions().  All of these defaults can be
       overridden at runtime, as indicated below.
  */

  ierr = KSPSetTolerances(ksp,1.e-2/((m+1)*(n1+1)),1.e-50,PETSC_DEFAULT,
                          PETSC_DEFAULT);CHKERRQ(ierr);
  /* 
    Set runtime options, e.g.,
        -ksp_type <type> -pc_type <type> -ksp_monitor -ksp_rtol <rtol>
    These options will override those specified above as long as
    KSPSetFromOptions() is called _after_ any other customization
    routines.
  */
  ierr = KSPSetFromOptions(ksp);CHKERRQ(ierr);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
                      Solve the linear system
     - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  PetscPreLoadBegin(PETSC_TRUE,"Solve System profiling");
  ierr = KSPSolve(ksp,b,x);CHKERRQ(ierr);
  PetscPreLoadEnd();

#if 1
  PetscObjectSetName((PetscObject)x,"temp");
  PetscViewer viewer;
  ierr = PetscViewerHDF5Open(PETSC_COMM_WORLD,"./temp.h5",
			     FILE_MODE_WRITE,&viewer); CHKERRQ(ierr);
  ierr = VecView(x,viewer);CHKERRQ(ierr);
  ierr = PetscViewerDestroy(&viewer);CHKERRQ(ierr);
#endif
  
  /*
     Free work space.  All PETSc objects should be destroyed when they
     are no longer needed.
  */
  ierr = KSPDestroy(&ksp);CHKERRQ(ierr);
  ierr = VecDestroy(&u);CHKERRQ(ierr);  
  ierr = VecDestroy(&x);CHKERRQ(ierr);
  ierr = VecDestroy(&b);CHKERRQ(ierr);  
  ierr = MatDestroy(&A);CHKERRQ(ierr);

  /*
     Always call PetscFinalize() before exiting a program.  This routine
       - finalizes the PETSc libraries as well as MPI
       - provides summary and diagnostic information if certain runtime
         options are chosen (e.g., -log_summary). 
  */
  ierr = PetscFinalize();CHKERRQ(ierr);
  return 0;
}
