#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pn_core/core.h"
#include "pn_core/config/config.h"
#include "pn_core/config/identity.h"
#include "pn_logger/logger.h"

int main(void)
{
    int retval = 1;

    struct PNCore *core = NULL;

    printf("Starting...\r\n");

    if(!core_new(&core))
    {
        printf("Failed at Core_new\r\n");
        goto exit;
    }

    if(!core_init(&core, 2048))
    {
        printf("Failed at Core_init\r\n");
        goto exit;
    }

    printf("[PeerID]\r\n%s\r\n[Base64'd Privkey]\r\n%s\r\n",
        core->config->identity->peer_id, core->config->b64privkey);

    retval = 0;

exit:
    if(core != NULL)
       core_free(core);

    fprintf(stderr, "Exiting...\r\n");

    return retval;
}
