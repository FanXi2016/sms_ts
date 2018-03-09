#ifndef _SMS_LOG_H_
#define _SMS_LOG_H_


#define SMS_SUCCESS (0)  /* Success */
#define SMS_ERR_NULL (-1)  /* NULL Pointer */
#define SMS_ERR_LENGTH (-2)  /* Empty Message lenght */
#define SMS_ERR_ADDR_NON_DIGIT (-3)  /* SMS Address Non-digit */
#define SMS_ERR_SUBMIT (-4)  /* Encode SMS to subimt format */
#define SMS_ERR_DELIVER (-5)  /* Decode PDU Messageto SMS. */

typedef enum {
    SMS_E_SUCCESS = 0,
    SMS_E_INVALID_PARAM = 1,   
    SMS_E_MESSAGE_TYPE_ERR = 2,
    SMS_E_VALIDITY_ERR = 3,
} e_sms_error_enum;


/*========================================*/
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
/*========================================*/

#endif /* _SMS_LOG_H_ */
