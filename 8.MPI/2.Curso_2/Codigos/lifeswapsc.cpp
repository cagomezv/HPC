int k; char *tmp;
for (j=i1; j<i2; j++) {
  // Computes new `j' row ...
  tmp = board[j-1];
  board[j-1] = row2;
  row2 = row1;
  row1 = tmp;
}
