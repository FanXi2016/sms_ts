


#define SMS_TS_PDU_MESSAGE_MAX_LEN 255


typedef enum {
    FALSE = 0;
    TRUE = 1;
} BOOLEAN;


typedef struct sms_ts_submit_gw_s {
  boolean                           reject_duplicates;        /* TP-RD */
  boolean                           reply_path_present;       /* TP-RP */
  boolean                           user_data_header_present; /* TP-UDHI */
  boolean                           status_report_enabled;    /* TP-SRR */
  wms_message_number_type           message_reference;        /* TP-MR */
  wms_address_s_type                address;                  /* TP-DA */
  wms_pid_e_type                    pid;                      /* TP-PID */
  wms_gw_dcs_s_type                 dcs;                      /* TP-DCS */
  wms_gw_validity_s_type            validity;                 /* TP-VPF & TP-VP */
  wms_gw_user_data_s_type           user_data;                /* TP-UD */
} sms_ts_submit_gw_s;

#define SMS_TS_TP_MTI_DELIVER 0;
#define SMS_TS_TP_MTI_SUBMIT 1;
#define SMS_TS_TP_MTI_STATUS_REPORT_OR_COMMAND 2;
#define SMS_TS_TP_MTI_RESERVED 3;


#define SMS_TS_TP_VPF_NOT_PRESENT 0;
#define SMS_TS_TP_VPF_ENHANCED 1;
#define SMS_TS_TP_VPF_RELATIVE 2;
#define SMS_TS_TP_VPF_ABSOLUTE 3;

typedef enum
{
    SMS_TS_TP_VPF_NOT_PRESENT = 0,
    SMS_TS_TP_VPF_ENHANCED = 1,
    SMS_TS_TP_VPF_RELATIVE = 2,
    SMS_TS_TP_VPF_ABSOLUTE = 3,
} sms_ts_reply_path_present_e_type;

typedef enum
{

} sms_ts_number_type_e_type;


typedef struct sms_ts_address_s {
    uint8 digit_mode:1;
    uint8 number_type:3;
    uint8 number_plan_identification:4;

    uint8 address_len;
    uint8 address[SMS_TS_ADDRESS_MAX_LEN];
} sms_ts_address_s_type;


typedef struct sms_ts_address_s {
    uint8 digit_mode:1;
    sms_ts_number_type_e_type number_type;
    uint8 number_plan_identification:4;

    uint8 address_len;
    uint8 address[SMS_TS_ADDRESS_MAX_LEN];
} sms_ts_address_s_type;



typedef struct sms_ts_gw_submit_s {
    /* message_type_indicator - Just SUBMIT. */
    BOOLEAN reject_duplicates;
    /* validity_period_format. */
    BOOLEAN reply_path;
    BOOLEAN user_data_header_present;
    BOOLEAN status_report_request;

    uint8 message_reference;

    sms_ts_address_s_type address;
}







typedef struct sms_ts_pdu_message_s
{
    uint32 message_len;
    uint8 message[SMS_TS_PDU_MESSAGE_MAX_LEN];
} sms_ts_pdu_message_s_type;



