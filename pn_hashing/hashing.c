#include <stdlib.h>
#include <string.h>

#include "pn_hashing/hashing.h"
#include "pn_logger/logger.h"
#include "libp2p/crypto/sha256.h"

int hash_sha256(unsigned char *key, unsigned char *nonce,
    const char *str, unsigned char hash[32])
{
    int szhash;
    size_t szstr = strlen(str),
            szkey = strlen((char*)key),
            sznonce = strlen((char*)nonce);

    mbedtls_sha256_context ctx;
    libp2p_crypto_hashing_sha256_init(&ctx);

    libp2p_crypto_hashing_sha256_update(&ctx, (const char*)key, szkey);
    libp2p_crypto_hashing_sha256_update(&ctx, (const char*)nonce, sznonce);

    if((szhash = libp2p_crypto_hashing_sha256(str, szstr, hash)) == 0)
    {
        free(*&hash);
        logger_msg(ERROR, "Unable to generate sha256 hash!");
    }

    libp2p_crypto_hashing_sha256_finish(&ctx, hash);
    libp2p_crypto_hashing_sha256_free(&ctx);

    return szhash;
}
