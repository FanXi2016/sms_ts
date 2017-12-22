#ifndef _SMS_CODING_H_
#define _SMS_CODING_H_

#include "sms_comdef.h"

#define SMS_PDU_MASSAGE_MAX_LEN 255


typedef struct sms_coding_pdu_message_s {
    uint32 message_len;
    uint8 message[SMS_PDU_MASSAGE_MAX_LEN];

} sms_coding_pdu_message_s_type;

/*
typedef enum {
    E_SMS_TAG_TYPE_UNKNOWN = 0,
    E_SMS_TAG_TYPE_MT_READ,
    E_SMS_TAG_TYPE_MT_UNREAD,
    E_SMS_TAG_TYPE_MO_SENT,
    E_SMS_TAG_TYPE_MO_UNSEND,
    E_SMS_TAG_TYPE_STATUS_REPORT,
    E_SMS_TAG_TYPE_MAX,
} e_sms_tag_type_enum;
*/

typedef enum {
    E_SMS_CONTENT_DCS_UNKNOWN = 0,
    E_SMS_CONTENT_DCS_GSM_7BIT,
    E_SMS_CONTENT_DCS_8BIT,
    E_SMS_CONTENT_DCS_USC2,
    E_SMS_CONTENT_DCS_MAX,
} e_sms_content_dcs_enum;

typedef struct sms_validity_period_s {
    uint8 year;
    uint8 month;
    uint8 date;
    uint8 hour;
    uint8 min;
    uint8 sec;
    uint8 timezone;
} sms_validity_period_s_type;


typedef struct sms_message_s {
    uint8 smsc[32];
    uint8 number[32];

    sms_validity_period_s_type validity;

    e_sms_content_dcs_enum content_dcs;
    uint32 content_len;
    uint8 content[256];
} sms_message_s_type;


extern int sms_coding_pdu_submit(char *address, char *content, uint32 content_len, sms_coding_pdu_message_s_type *pdu);
extern int sms_decode_pdu_deliver(uint8 *pdu, uint32 pdu_len, sms_message_s_type *sms_message);


#endif /* _SMS_CODING_H_ */
