#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pn_core/config/config.h"
#include "pn_core/config/identity.h"
#include "libp2p/utils/vector.h"
#include "multiaddr/multiaddr.h"
#include "libp2p/crypto/key.h"
#include "libp2p/crypto/encoding/base64.h"

/***
 * public
 */
int core_config_is_valid_identity(struct PNIdentity *identity)
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
int core_config_init(struct PNConfig **config, unsigned int num_bits_for_keypair, int swarm_port)
{
    if(!core_config_identity_new(&((*config)->identity)))
    {
        printf("Failed at Core_config_identity_new\r\n");
        return 0;
    }

    int retval = 0, tries = 1;

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
            tries++;
            continue;
        }
    } while(retval == 0 && tries != 5);

    if(tries == 5 && retval == 0)
        return 0;

    struct PrivateKey *privkey = libp2p_crypto_private_key_new();
    if(privkey == NULL)
    {
        fprintf(stderr, "Unable to new privkey...\r\n");
        return 0;
    }

    privkey->data_size = (*config)->identity->private_key.der_length;
    privkey->data = (unsigned char*)malloc(privkey->data_size);
    if(privkey->data == NULL)
    {
        fprintf(stderr, "Unable to malloc privkey->data...\r\n");
        libp2p_crypto_private_key_free(privkey);
        return 0;
    }

    memcpy(privkey->data, (*config)->identity->private_key.der, privkey->data_size);
    privkey->type = KEYTYPE_RSA;

    // Protobuf it
    size_t protobuf_size = libp2p_crypto_private_key_protobuf_encode_size(privkey);
    unsigned char protobuf[protobuf_size];
    retval = libp2p_crypto_private_key_protobuf_encode(privkey, protobuf, protobuf_size, &protobuf_size);
    libp2p_crypto_private_key_free(privkey);
    if(retval == 0)
    {
        fprintf(stderr, "Unable to protobuf encode data...\r\n");
        return 0;
    }

    // Then base64 it
    size_t encoded_size = libp2p_crypto_encoding_base64_encode_size(protobuf_size);
    unsigned char encoded_buffer[encoded_size + 1];
    retval = libp2p_crypto_encoding_base64_encode(protobuf, protobuf_size, encoded_buffer, encoded_size, &encoded_size);
    if(retval == 0)
    {
        fprintf(stderr, "Unable to base64 encode data...\r\n");
        return 0;
    }

    encoded_buffer[encoded_size] = 0;

    (*config)->b64privkey = (unsigned char*)malloc(encoded_size + 1);
    if((*config)->b64privkey == NULL)
    {
        fprintf(stderr, "Unable to malloc (*config)->b64privkey...\r\n");
        return 0;
    }

    memcpy((*config)->b64privkey, encoded_buffer, encoded_size + 1);

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

    //(*config)->addresses = NULL;
    //(*config)->gateway = NULL;
    //(*config)->bootstrap_peers = NULL;
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
