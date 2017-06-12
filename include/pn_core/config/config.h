#ifndef _PN_CORE_CONFIG_H_
#define _PN_CORE_CONFIG_H_

#include "pn_core/config/identity.h"
#include "libp2p/utils/vector.h"

struct PNConfig
{
    unsigned char *b64privkey;
    struct PNIdentity *identity;
    //struct Addresses *addresses;
    //struct Gateway *gateway;
    struct Libp2pVector *bootstrap_peers;
};

int core_config_is_valid_identity(struct PNIdentity *identity);

int core_config_init(struct PNConfig **config, unsigned int num_bits_for_keypair, int swarm_port);

int core_config_new(struct PNConfig **config);

int core_config_free(struct PNConfig *config);

#endif
