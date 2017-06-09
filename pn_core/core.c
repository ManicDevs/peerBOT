#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pn_core/core.h"
#include "pn_core/config/config.h"
#include "pn_logger/logger.h"
#include "libp2p/crypto/key.h"
#include "libp2p/crypto/encoding/base64.h"

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
    if(!core_config_init(&((*core)->config), num_bits_for_keypair))
    {
        fprintf(stderr, "Unable to init config...\r\n");
        return 0;
    }

    struct PrivateKey *privkey = libp2p_crypto_private_key_new();
    if(privkey == NULL)
    {
        fprintf(stderr, "Unable to new privkey...\r\n");
        return 0;
    }

    privkey->data_size = (*core)->config->identity->private_key.der_length;
    privkey->data = (unsigned char*)malloc(privkey->data_size);
    if(privkey->data == NULL)
    {
        fprintf(stderr, "Unable to malloc privkey->data...\r\n");
        libp2p_crypto_private_key_free(privkey);
        return 0;
    }

    memcpy(privkey->data, (*core)->config->identity->private_key.der, privkey->data_size);
    privkey->type = KEYTYPE_RSA;

    int retval = 0;

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

    (*core)->config->b64privkey = (unsigned char*)malloc(encoded_size);
    memset((*core)->config->b64privkey, 0, encoded_size);
    memcpy((*core)->config->b64privkey, encoded_buffer, encoded_size);

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
