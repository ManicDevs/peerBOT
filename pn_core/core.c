#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pn_core/core.h"
#include "pn_core/config/config.h"
#include "pn_logger/logger.h"
#include "libp2p/utils/logger.h"

int core_init(struct PNCore **core, unsigned int num_bits_for_keypair)
{
    if(!logger_init())
    {
        fprintf(stderr, "[Error] Logger initalization: failed\r\n");
        return 0;
    }

    if(!core_config_new(&((*core)->config)))
    {
        fprintf(stderr, "Unable to new config...\r\n");
        return 0;
    }

    printf("Generating 1024-bit RSA Keypair: ");
    if(!core_config_init(&((*core)->config), num_bits_for_keypair))
    {
        fprintf(stderr, "Unable to init config...\r\n");
        return 0;
    }

    printf("done\r\n");

    return 1;
}

int core_new(struct PNCore **core)
{
    *core = (struct PNCore*)malloc(sizeof(struct PNCore));
    if(*core == NULL)
        return 0;

    memset(*core, 0, sizeof(struct PNCore));

    (*core)->config = NULL;

    return 1;
}

int core_free(struct PNCore *core)
{
    if(core != NULL)
    {
        if(core->config != NULL)
            core_config_free(core->config);

        if(logger_initialized())
            logger_free();

        free(core);
    }

    return 1;
}
