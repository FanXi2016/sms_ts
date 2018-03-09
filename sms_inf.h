#ifndef _SMS_INF_H_
#define _SMS_INF_H_


#ifndef _UINT8_DEFINED
typedef unsigned char uint8;
#define _UINT8_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef unsigned short uint16;
#define _UINT16_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef unsigned int uint32;
#define _UINT32_DEFINED
#endif


#define PDU_MASSAGE_MAX_LEN 255
#define SMS_ADDRESS_MAX_LEN 32
#define SMS_CONTENT_MAX_LEN 256


typedef enum {
    E_CONTENT_DCS_UNKNOWN = 0,
    E_CONTENT_DCS_ASCII,
    E_CONTENT_DCS_UTF8,
    E_CONTENT_DCS_MAX,
} e_content_dcs_enum;

typedef struct pdu_message_s {
    uint32 message_len;
    uint8 message[PDU_MASSAGE_MAX_LEN];
} pdu_message_s_type;

typedef struct sms_message_s {
    uint8 debug;
    uint8 smsc_len;
    uint8 smsc[SMS_ADDRESS_MAX_LEN];
    uint8 addr_len;
    uint8 address[SMS_ADDRESS_MAX_LEN];
    uint8 validity;
    uint8 content_dcs;
    uint32 content_len;
    uint8 content[SMS_CONTENT_MAX_LEN];
} sms_message_s_type;

#endif /* _SMS_INF_H_ */
