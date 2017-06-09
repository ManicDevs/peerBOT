#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pn_core/core.h"
#include "pn_core/config/config.h"
#include "pn_core/config/identity.h"
#include "pn_logger/logger.h"

struct PNCore *core = NULL;

int main(void)
{
    int retval = 1;

    printf("Starting...\r\n");

    if(!core_new(&core))
    {
        printf("Failed at Core_new\r\n");
        goto exit;
    }

    if(!core_init(&core, 1024))
    {
        printf("Failed at Core_init\r\n");
        goto exit;
    }

    retval = 0;

exit:
    if(core != NULL)
       core_free(core);

    fprintf(stderr, "Exiting...\r\n");

    return retval;
}
