#ifndef _PN_HASHING_H_
#define _PN_HASHING_H_

#include "pn_hashing/utils.h"

#define HASHING_KEY "D34DB33F"

int hash_sha256(unsigned char *key, unsigned char *nonce,
    const char *str, unsigned char hash[32]);

#endif
