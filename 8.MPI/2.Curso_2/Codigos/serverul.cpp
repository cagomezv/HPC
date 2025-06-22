proc_status[1..P-1] =0;
j=0
while (true) {
      Immediate Receive len,tag->k, proc
      while(j<N) {
         if (received) break;
	 Process x_j -> r_j
         j++
       }
      Wait until received;
      if (len==0) {
	 extrae r_k,...,r_{k+len-1}
      } else {
	 proc_status[proc]++;
      }
      jj = min(N,j+chunksize);
      len = jj-j;
      Send x_j,x_{j+1}, x_{j+len-1}, tag=j to proc
      // Verifica si todos terminaron
      Si proc_status[proc] == 2 para proc=1..P-1 then break;
}
