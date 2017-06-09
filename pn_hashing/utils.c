#include <stdlib.h>
#include <string.h>

#include "pn_hashing/utils.h"
#include "libp2p/secio/secio.h"

int hash_generate_nonce(char results, int sznonce)
{
    int retval = 0;

    libp2p_secio_generate_nonce()

    if(!libp2p_secio_generate_nonce(&loc_sess->local_nonce[0], sznonce))
        goto exit;

    retval = 1;

exit:

    return retval;
}
