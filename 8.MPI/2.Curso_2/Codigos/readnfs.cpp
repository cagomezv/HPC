FILE *fid = fopen("options.dat","r");
int N; double mass;
fscanf(fid,"%d",&N);
fscanf(fid,"%lf",&mass);
fclose(fid);
