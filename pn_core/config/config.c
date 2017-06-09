#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pn_core/config/config.h"
#include "pn_core/config/identity.h"

/***
 * public
 */

int core_config_is_valid_identity(struct Identity *identity)
{
    if(identity->peer_id == NULL || identity->peer_id[0] != 'Q' || identity->peer_id[1] != 'm')
        return 0;

    return 1;
}

/***
 * create a configuration based on the passed in parameters
 * @param config the configuration struct to be filled in
 * @param num_bits_for_keypair number of bits for the key pair
 * @returns true(1) on success, otherwise 0
 */
int core_config_init(struct PNConfig **config, unsigned int num_bits_for_keypair)
{
    if(!core_config_identity_new(&((*config)->identity)))
    {
        printf("Failed at Core_config_identity_new\r\n");
        return 0;
    }

    int retval = 0;

    do
    {
        if(!core_config_identity_init((*config)->identity, num_bits_for_keypair))
        {
            printf("Failed at Core_config_identity_init\r\n");
            return 0;
        }

        if(!(retval = core_config_is_valid_identity((*config)->identity)))
        {
            core_config_identity_free((*config)->identity);
            continue;
        }
    } while(retval == 0);

    return 1;
}

/***
 * Initialize memory for a RepoConfig struct
 * @param config the structure to initialize
 * @returns true(1) on success
 */
int core_config_new(struct PNConfig **config)
{
    *config = (struct PNConfig*)malloc(sizeof(struct PNConfig));
    if(*config == NULL)
        return 0;

    memset(*config, 0, sizeof(struct PNConfig));

    (*config)->identity = NULL;
    (*config)->b64privkey = NULL;

    return 1;
}

/**
 * Free resources
 * @param config the struct to be freed
 * @returns true(1) on success
 */
int core_config_free(struct PNConfig *config)
{
    if(config != NULL)
    {
        if(config->b64privkey != NULL)
            free(config->b64privkey);
        if(config->identity != NULL)
            core_config_identity_free(config->identity);

        free(config);
    }

    return 1;
}
