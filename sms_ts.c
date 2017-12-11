/*
 * valeod.c
 * Derived from simpleDialer.c
 *
 *  Created on: 07.12.2017
 *      Author: XI Fan
 */


 #include "sms_ts.h"


#define SMS_TS_ADDRESS_MAX_LEN 48

#define SMS_TS_TP_MTI_DELIVER 0;
#define SMS_TS_TP_MTI_SUBMIT 1;
#define SMS_TS_TP_MTI_STATUS_REPORT_OR_COMMAND 2;
#define SMS_TS_TP_MTI_RESERVED 3;


#define SMS_TS_DIGIT_MODE_4_BIT 0
#define SMS_TS_DIGIT_MODE_8_BIT 1


typedef struct sms_ts_address_s {
    uint8 address_len;

    uint8 digit_mode:1;
    uint8 number_type:3;
    uint8 number_plan_identification:4;

    uint8 address[SMS_TS_ADDRESS_MAX_LEN];
} sms_ts_address_s_type;

typedef struct sms_ts_submit_head_s {
    uint8 message_type_indicator : 2;    /* TP-MTI */
    uint8 reject_duplicates : 1;         /* TP-RD */
    uint8 validity_period_format : 2;    /* TP-VPF */
    uint8 reply_path : 1;                /* TP-RP */
    uint8 user_data_header_present : 1;  /* TP-UDHI */
    uint8 status_report_request : 1;     /* TP-SRR */
} sms_ts_submit_head_s_type;

/* 3GPP TS 23.040 9.2.2.2 */
typedef struct sms_ts_pdu_submit_s {
    sms_ts_submit_head_s_type head;
    uint8 message_reference;

    sms_ts_address_s_type destination_address;

    uint8 protocol_identifier;
    uint8 data_coding_scheme;
    uint8 validity_period;

    uint8 user_data_len;
    uint8 user_data[];
} sms_ts_pdu_submit_s_type;



 
/** @brief sms_ts_pdu_encode_submit
 *
 *  [Description]
 *
 *  @param[in]
 *
 *  @return
 */
wms_status_e_type sms_ts_pdu_encode_submit(const wms_gw_submit_s_type *sms_submit_data, sms_ts_pdu_message_s_type *pdu)
{
    sms_ts_pdu_submit_s_type submit;


    if (sms_submit_data == NULL || pdu == NULL) {
        ERR("Null pointer in wms_ts_decode_submit!");
        return 0;
    }

    /* Clear PDU. */
    memset(submit, 0, sizeof(sms_ts_pdu_submit_s_type));
    memset(pdu, 0, sizeof(sms_ts_pdu_message_s_type));


    /* TP-MTI, TP-RD, TP-VPF, TP-RP, TP_UDHI, TP-SRR */
    submit.status_report_request = 0;  /* bit7 */
    submit.user_data_header_present = 0;  /* bit6 */
    submit.reply_path = 0;  /* bit 5 */
    submit.validity_period_format = SMS_TS_TP_VPF_RELATIVE;  /* bit[4..3] */
    submit.reject_duplicates = 0;  /* bit2 */
    submit.message_type_indicator = SMS_TS_TP_MTI_SUBMIT;  /* bit[1..0] */

    submit.message_reference = 0x00;


    
  submit->reject_duplicates         = (data[pos] & 0x04) ? TRUE : FALSE;
                                      /* bit 2 */
  submit->validity.format           = (wms_gw_validity_format_e_type) (( data[pos] & 0x18 ) >> 3); /* bits 3, 4 */
  submit->status_report_enabled     = (data[pos] & 0x20) ? TRUE : FALSE;
                                      /* bit 5 */
  submit->user_data_header_present  = (data[pos] & 0x40) ? TRUE : FALSE;
                                      /* bit 6 */
  submit->reply_path_present        = (data[pos] & 0x80) ? TRUE : FALSE;
                                      /* bit 7 */
  pos ++;


  /* TP-MR
  */
  submit->message_reference = data[pos];
  pos ++;

  /* TP-DA
  */
  i = wms_ts_decode_address( & data[pos], & submit->address );
  if( i == 0 )
  {
    return WMS_INVALID_PARM_SIZE_S;
  }

  pos += i;

  /* TP-PID
  */
  submit->pid = (wms_pid_e_type) data[pos];
  pos ++;

  /* TP-DCS
  */
  pos += wms_ts_decode_dcs( data+pos, & submit->dcs );

  /* TP-VP
  */
  i = wms_ts_decode_gw_validity( data+pos, & submit->validity );
  if ((submit->validity.format != WMS_GW_VALIDITY_NONE) && ( i == 0 ))
  {
    return WMS_INVALID_PARM_VALUE_S;
  }

  pos += i;

  /* TP-UDL
  */
//  submit->user_data_len = data[pos];
  pos ++;

  /* TP-UD
  */
  i = wms_ts_decode_gw_user_data( & submit->dcs,
                                       data[pos-1],
                                       data+pos,
                                       submit->user_data_header_present,
                                     & submit->user_data );

  if (i > WMS_SMS_UDL_MAX_8_BIT)
  {
    LOGE("User Data Length has exceeded capacity: UDL = %lu", i);
    st = WMS_INVALID_USER_DATA_SIZE_S;
  }

  pos += i;

  /* Set the global raw ts data len
  */
  raw_ts_len = pos;

  return st;

} /* wms_ts_decode_submit() */