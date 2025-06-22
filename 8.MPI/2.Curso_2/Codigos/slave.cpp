Send len=0 to server
while(true) {
     Receive len,tag=k
     if (len=0) break;
     Process x_k,x_{k+1},...,x_{k+len-1} ->
                         r_k,r_{k+1},...,r_{k+len-1}
     Send r_k,r_{k+1},...,r_{k+len-1} to server
}
Send len=0  to server
