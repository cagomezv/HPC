Mat A(100,100);
double *a, *a10;
// Pointer to the internal storage area
a = &A(0,0);
// Pointer to the internal storage area, 
// starting at row 10 (0 base)
a10 = &A(10,0);
