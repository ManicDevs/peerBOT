#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pn_core/core.h"
#include "pn_core/node.h"
#include "pn_core/config/config.h"
#include "pn_routing/routing.h"
#include "pn_logger/logger.h"

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

    printf("Generating %d-bit RSA Keypair: ", num_bits_for_keypair);
    if(!core_config_init(&((*core)->config), num_bits_for_keypair, 4011))
    {
        fprintf(stderr, "Unable to init config...\r\n");
        return 0;
    }

    printf("done\r\n");

    return 1;

    if(!core_node_new(&((*core)->node)))
    {
        fprintf(stderr, "Unable to new node...\r\n");
        return 0;
    }

    if(!core_node_init(&((*core)->node), (*core)->config))
    {
        fprintf(stderr, "Unable to init node...\r\n");
        return 0;
    }

    return 1;
}

int core_new(struct PNCore **core)
{
    *core = (struct PNCore*)malloc(sizeof(struct PNCore));
    if(*core == NULL)
        return 0;

    memset(*core, 0, sizeof(struct PNCore));

    (*core)->config = NULL;
    (*core)->node = NULL;
    //(*core)->routing = NULL;

    return 1;
}

int core_free(struct PNCore *core)
{
    if(core != NULL)
    {
        if(core->config != NULL)
            core_config_free(core->config);

        if(core->node != NULL)
            core_node_free(core->node);

        if(logger_initialized())
            logger_free();

        free(core);
    }

    return 1;
}
