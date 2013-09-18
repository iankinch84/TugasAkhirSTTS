/* 
 * File:   mod_killbots.h
 * Author: ian
 *
 * Created on June 27, 2013, 9:57 AM
 */

#ifndef MOD_KILLBOTS_H
#define	MOD_KILLBOTS_H

#define BF_FOUND 1
#define BF_NOT_FOUND 0

static unsigned int BIT_SHIFT;


#define ENCRYPTION_MODE 1
#define DECRYPTION_MODE 0

typedef struct {
	unsigned char k[8];
	unsigned char c[4];
	unsigned char d[4];
} key_set;

#endif	/* MOD_KILLBOTS_H */

