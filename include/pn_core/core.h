#ifndef _PN_CORE_H_
#define _PN_CORE_H_

struct PNCore
{
    struct PNConfig *config;
};

int core_init(struct PNCore **core, unsigned int num_bits_for_keypair);

int core_new(struct PNCore **core);

int core_free(struct PNCore *core);

#endif
