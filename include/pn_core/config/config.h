#ifndef _PN_CORE_CONFIG_H_
#define _PN_CORE_CONFIG_H_

#include "pn_core/config/identity.h"

struct PNConfig
{
    struct Identity *identity;
};

int core_config_is_valid_identity(struct Identity *identity);

int core_config_init(struct PNConfig **config, unsigned int num_bits_for_keypair);

int core_config_new(struct PNConfig **config);

int core_config_free(struct PNConfig *config);

#endif
