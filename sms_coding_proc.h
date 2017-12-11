#ifndef SMS_CODING_PROC_H_
#define SMS_CODING_PROC_H_

#include "comdef.h"


#define SMS_PDU_MASSAGE_MAX_LEN 255


typedef struct sms_coding_pdu_message_s {
    uint32 message_len;
    uint8 message[SMS_PDU_MASSAGE_MAX_LEN];

} sms_coding_pdu_message_s_type;


extern int sms_coding_pdu_submit(char *dest_address, char *user_data, uint32 user_data_len, sms_coding_pdu_message_s_type *pdu);


#endif /* SMS_CODING_PROC_H_ */
