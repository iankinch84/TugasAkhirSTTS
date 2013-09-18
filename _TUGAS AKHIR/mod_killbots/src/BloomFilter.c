#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define FILTER_SIZE 20
static unsigned int NUM_HASHES  = 7;
static unsigned int MAX_BIT = 32;

#define FILTER_BYTE_SIZE (1 << (FILTER_SIZE - 3))
#define FILTER_BITMASK 0

void bf_insert(unsigned char filter[], char *str);
int bf_is_any(unsigned char filter[], char *str);
void bf_hash_code(unsigned int hash[], char *in);

unsigned int RSHash  (unsigned char *, unsigned int);
unsigned int DJBHash (unsigned char *, unsigned int);
unsigned int FNVHash (unsigned char *, unsigned int);
unsigned int JSHash  (unsigned char *, unsigned int);
unsigned int PJWHash (unsigned char *, unsigned int);
unsigned int SDBMHash(unsigned char *, unsigned int);
unsigned int DEKHash (unsigned char *, unsigned int);

unsigned int get_FILTER_BYTE_SIZE(unsigned int fs){
    return (1 << (FILTER_SIZE - 3));
}

unsigned int get_FILTER_BITMASK(unsigned int fs){
    return ((1 << FILTER_SIZE) - 1);
}

void bf_insert(unsigned char filter[], char *str)
{
	unsigned int hash[NUM_HASHES];
	int i;
	
	bf_hash_code(hash, str);
	
	for (i = 0; i < NUM_HASHES; i++) {
            /* xor-fold the hash into FILTER_SIZE bits */
            hash[i] = (hash[i] >> FILTER_SIZE) ^ 
                      (hash[i] & FILTER_BITMASK);
            /* set the bit in the filter */
            filter[hash[i] >> 3] |= 1 << (hash[i] & 7);
		
	}
}

int bf_is_any(unsigned char filter[], char *str)
{
	unsigned int hash[NUM_HASHES];
	int i;
	
	bf_hash_code(hash, str);
	
	for (i = 0; i < NUM_HASHES; i++) {
		hash[i] = (hash[i] >> FILTER_SIZE) ^
		          (hash[i] & FILTER_BITMASK);
		if (!(filter[hash[i] >> 3] & (1 << (hash[i] & 7))))
			return 0;
	}
	
	return 1;
}

void bf_hash_code(unsigned int hash[], char *in)
{
	unsigned char *str = (unsigned char *)in;
	int pos = strlen(in);
        
        hash[0] = RSHash  (str, pos);
	hash[1] = DJBHash (str, pos);
	hash[2] = FNVHash (str, pos);
	hash[3] = JSHash  (str, pos);
	hash[4] = PJWHash (str, pos);
	hash[5] = SDBMHash(str, pos);
	hash[6] = DEKHash (str, pos);
}



/****************\
| Hash Functions |
\****************/

unsigned int RSHash(unsigned char *str, unsigned int len)
{
   unsigned int b    = 378551;
   unsigned int a    = 63689;
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = hash * a + (*str);
      a    = a * b;
   }

   return (hash);
}

unsigned int JSHash(unsigned char *str, unsigned int len)
{
   unsigned int hash = 1315423911;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash ^= ((hash << 5) + (*str) + (hash >> 2));
   }

   return hash;
}

unsigned int PJWHash(unsigned char *str, unsigned int len)
{
   const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
   const unsigned int ThreeQuarters     = (unsigned int)((BitsInUnsignedInt  * 3) / 4);
   const unsigned int OneEighth         = (unsigned int)(BitsInUnsignedInt / 8);
   const unsigned int HighBits          = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
   unsigned int hash              = 0;
   unsigned int test              = 0;
   unsigned int i                 = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = (hash << OneEighth) + (*str);

      if((test = hash & HighBits)  != 0)
      {
         hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
      }
   }

   return hash;
}

unsigned int SDBMHash(unsigned char *str, unsigned int len)
{
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = (*str) + (hash << 6) + (hash << 16) - hash;
   }

   return hash;
}

unsigned int DJBHash(unsigned char *str, unsigned int len)
{
   unsigned int hash = 5381;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = ((hash << 5) + hash) + (*str);
   }

   return hash;
}

unsigned int DEKHash(unsigned char *str, unsigned int len)
{
   unsigned int hash = len;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
   }
   return hash;
}

unsigned int FNVHash(unsigned char *str, unsigned int len)
{
   const unsigned int fnv_prime = 0x811C9DC5;
   unsigned int hash      = 0;
   unsigned int i         = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash *= fnv_prime;
      hash ^= (*str);
   }

   return hash;
}
