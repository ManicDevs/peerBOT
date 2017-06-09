#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pn_core/config/identity.h"
#include "pn_logger/logger.h"
#include "libp2p/crypto/rsa.h"
#include "libp2p/crypto/key.h"
#include "libp2p/crypto/encoding/base64.h"
#include "libp2p/crypto/encoding/x509.h"
#include "libp2p/crypto/peerutils.h"

int core_config_identity_generate_peerid(struct Identity *identity)
{
    struct PublicKey public_key;

    public_key.data = (unsigned char*)identity->private_key.public_key_der;
    public_key.data_size = identity->private_key.public_key_length;
    public_key.type = KEYTYPE_RSA;

    if(!libp2p_crypto_public_key_to_peer_id(&public_key, &identity->peer_id))
        return 0;

    return 1;
}

/* Public methods */
/***
 * Initializes a new Identity. NOTE: This builds a new private/public keypair
 * @param identity the identity to fill
 * @param num_bits_for_keypair the number of bits for the keypair
 * @returns true(1) on success, false(0) otherwise
 */
int core_config_identity_init(struct Identity *identity, unsigned long num_bits_for_keypair)
{
    if(num_bits_for_keypair < 1024)
        return 0;

    // Generate the private and public keys
    if(!libp2p_crypto_rsa_generate_keypair(&(identity->private_key), num_bits_for_keypair))
    {
        libp2p_crypto_rsa_rsa_private_key_free(&identity->private_key);
        return 0;
    }

    if(!core_config_identity_generate_peerid(identity))
    {
        libp2p_crypto_rsa_rsa_private_key_free(&(identity->private_key));
        return 0;
    }

    return 1;
}

/***
 * Build a RsaPrivateKey struct from a base64 string of the private key
 * @param identity where to put the new struct
 * @param base64 the null terminated base 64 encoded private key in DER format
 * @returns true(1) on success
 */
int core_config_identity_build_private_key(struct Identity *identity, const char *base64)
{
    int retval;

    struct PrivateKey *priv_key;

    priv_key = libp2p_crypto_private_key_new();
    if(priv_key == NULL)
    {
        logger_msg(ERROR, "Crypto PrivKey Init: failed");
        return 0;
    }

    priv_key->data_size = identity->private_key.der_length;
    priv_key->data = (unsigned char*)malloc(priv_key->data_size);
    if(priv_key->data == NULL)
    {
        logger_msg(ERROR, "Crypto PrivKey Data: failed");
        libp2p_crypto_private_key_free(priv_key);
        return 0;
    }

    memcpy(priv_key->data, identity->private_key.der, priv_key->data_size);
    priv_key->type = KEYTYPE_RSA;

    size_t protobuf_size = libp2p_crypto_private_key_protobuf_encode_size(priv_key);
    unsigned char protobuf[protobuf_size];

    libp2p_crypto_private_key_protobuf_encode(priv_key, protobuf,
        protobuf_size, &protobuf_size);

    // Then base64 it
    size_t encoded_size = libp2p_crypto_encoding_base64_encode_size(protobuf_size);
    unsigned char encoded_buffer[encoded_size + 1];

    retval = libp2p_crypto_encoding_base64_encode(protobuf, protobuf_size,
        encoded_buffer, encoded_size, &encoded_size);
    if(retval == 0)
    {
        logger_msg(ERROR, "Crypto Base64 Encoding: failed");
        return 0;
    }

    encoded_buffer[encoded_size] = 0;

    // Now convert DER to RsaPrivateKey
    retval = libp2p_crypto_encoding_x509_der_to_private_key(priv_key->data,
        priv_key->data_size, &identity->private_key);
    libp2p_crypto_private_key_free(priv_key);
    if(retval == 0)
    {
        logger_msg(ERROR, "x509 DER to PrivKey: failed");
        return 0;
    }

    // Now build the Private key DER
    retval = libp2p_crypto_rsa_private_key_fill_public_key(&identity->private_key);
    if(retval == 0)
    {
        logger_msg(ERROR, "PrivKey fill PubKey: failed");
        return 0;
    }

    // Now build PeerID
    retval = core_config_identity_generate_peerid(identity);

    return retval;
}

int core_config_identity_new(struct Identity **identity)
{
    *identity = (struct Identity*)malloc(sizeof(struct Identity));
    if(identity == NULL)
        return 0;

    memset(*identity, 0, sizeof(struct Identity));

    (*identity)->peer_id = NULL;
    (*identity)->private_key.der = NULL;
    (*identity)->private_key.public_key_der = NULL;

    return 1;
}

int core_config_identity_free(struct Identity *identity)
{
    if(identity != NULL)
    {
        if(identity->private_key.public_key_der != NULL)
            free(identity->private_key.public_key_der);

        if(identity->private_key.der != NULL)
            free(identity->private_key.der);

        if(identity->peer_id != NULL)
            free(identity->peer_id);

        free(identity);
    }

    return 1;
}
