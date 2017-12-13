#ifndef _SMS_LOG_H_
#define _SMS_LOG_H_

/* SMS Marco */
#define SMS_LOG

#ifdef SMS_LOG
#include <sys/syslog.h>
#else
#endif


#ifdef SMS_LOG
#ifndef SMS_LOG_TAG
#define SMS_ERR(x...)  syslog(LOG_ERR, x)
#define SMS_INFO(x...)  syslog(LOG_INFO, x)
#define SMS_DEBUG(x...)  syslog(LOG_DEBUG, x)
#else
#define SMS_ERR(tag,...) \
do { \
    syslog(LOG_ERR,__VA_ARGS__); \
} while (0)

#define SMS_INFO(tag,...) \
do { \
    syslog(LOG_INFO,__VA_ARGS__); \
} while (0)

#define SMS_DEBUG(tag,...) \
do { \
    syslog(LOG_DEBUG,__VA_ARGS__); \
} while (0)
#endif
#else
#define SMS_ERR(tag,...) do {} while(0)
#define SMS_INFO(tag,...) do {} while(0)
#define SMS_DEBUG(tag,...) do {} while(0)
#endif


#endif /* _SMS_LOG_H_ */
