// report last pi(n) computed
int pi=0, last_reported = 0;
while (!received.empty()) {
  // Si el primero de `received' es `last_reported'
  // entonces sacarlo de `received' y reportarlo
  set<chunk_info>::iterator q = received.begin();
  if (q->start != last_reported) break;
  pi += q->primes;
  last_reported += q->checked;
  received.erase(q);
  printf("pi(%d) = %d (encolados %d)\n",
	 last_reported,pi,received.size())
}
