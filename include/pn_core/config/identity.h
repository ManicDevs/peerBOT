#ifndef _PN_CORE_CONFIG_IDENTITY_H_
#define _PN_CORE_CONFIG_IDENTITY_H_

#include "libp2p/crypto/rsa.h"

struct PNIdentity
{
    char *peer_id; // a pretty-printed hash of the public key
    struct RsaPrivateKey private_key; // a private key
};

int core_config_identity_init(struct PNIdentity *identity, unsigned long num_bits_for_keypair);

int core_config_identity_build_private_key(struct PNIdentity *identity, const char *base64);

int core_config_identity_new(struct PNIdentity **identity);

int core_config_identity_free(struct PNIdentity *identity);

#endif
