#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "pn_logger/logger.h"
#include "libp2p/utils/logger.h"

void logger_msg(int type, const char *fmt, ...)
{
    char /*msgbuf[] = "",*/ *ctype;

    //msgbuf[0] = '\0';

    //va_list ap;
    //va_start(ap, fmt);
    //vsprintf(msgbuf, fmt, ap);

    switch(type)
    {
        case INFO:
            ctype = "INFO";
            //libp2p_logger_info(ctype, "[%s]: %s\r\n", ctype, msgbuf);
        break;

        case DEBUG:
            ctype = "DEBUG";
            //libp2p_logger_debug(ctype, "[%s]: %s\r\n", ctype, msgbuf);
        break;

        case ERROR:
            ctype = "ERROR";
            //libp2p_logger_error(ctype, "[%s]: %s\r\n", ctype, msgbuf);
        break;
    }

    printf("[%s]: %s\r\n", ctype, fmt);

    //va_end(ap);
}

int logger_initialized(void)
{
    if(!libp2p_logger_initialized())
        return 0;

    return 1;
}


int logger_init(void)
{
    libp2p_logger_init();

    if(libp2p_logger_initialized() != 1)
        return 0;

    return 1;
}

int logger_free(void)
{
    if(!libp2p_logger_free())
        return 0;

    return 1;
}
