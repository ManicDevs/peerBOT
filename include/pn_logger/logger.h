#ifndef _PN_LOGGER_H_
#define _PN_LOGGER_H_

#define INFO 0
#define DEBUG 1
#define ERROR 2

void logger_msg(int type, const char *fmt, ...);

int logger_initialized(void);

int logger_init(void);

int logger_free(void);

#endif
