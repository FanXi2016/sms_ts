#ifndef _SMS_TS_H_
#define _SMS_TS_H_


#define SMSTS_ADDRESS_MAX_LEN 48
#define SMSTS_USER_DATA_MAX_LEN 255


#define SMSTS_MESSAGE_TYPE_DELIVER        0x00
#define SMSTS_MESSAGE_TYPE_DELIVER_REPORT 0x00
#define SMSTS_MESSAGE_TYPE_SUBMIT         0x01
#define SMSTS_MESSAGE_TYPE_SUBMIT_REPORT  0x01
#define SMSTS_MESSAGE_TYPE_COMMAND        0x02
#define SMSTS_MESSAGE_TYPE_STATUS_REPORT  0x02
#define SMSTS_MESSAGE_TYPE_RESERVED       0x03

typedef enum {
    E_SMSTS_DIGIT_MODE_4_BIT = 0,
    E_SMSTS_DIGIT_MODE_8_BIT = 1,
    E_SMSTS_DIGIT_MODE_MAX,
} e_smsts_digit_mode_enum;

typedef enum {
    E_SMSTS_NUMBER_TYPE_UNKNOWN       = 0,
    E_SMSTS_NUMBER_TYPE_INTERNATIONAL = 1,
    E_SMSTS_NUMBER_TYPE_NATIONAL      = 2,
    E_SMSTS_NUMBER_TYPE_NETWORK       = 3,
    E_SMSTS_NUMBER_TYPE_SUBSCRIBER    = 4,
    E_SMSTS_NUMBER_TYPE_ALPHANUMERIC  = 5,
    E_SMSTS_NUMBER_TYPE_ABBREVIATED   = 6,
    E_SMSTS_NUMBER_TYPE_RESERVED_7    = 7,
    E_SMSTS_NUMBER_TYPE_MAX,
} e_smsts_number_type_enum;

typedef enum {
    E_SMSTS_NUMBER_PLAN_UNKNOW      = 0,
    E_SMSTS_NUMBER_PLAN_TELEPHONY   = 1,
    E_SMSTS_NUMBER_PLAN_RESERVED_2  = 2,
    E_SMSTS_NUMBER_PLAN_DATA        = 3,
    E_SMSTS_NUMBER_PLAN_TELEX       = 4,
    E_SMSTS_NUMBER_PLAN_RESERVED_5  = 5,
    E_SMSTS_NUMBER_PLAN_RESERVED_6  = 6,
    E_SMSTS_NUMBER_PLAN_RESERVED_7  = 7,
    E_SMSTS_NUMBER_PLAN_NATIONAL    = 8,
    E_SMSTS_NUMBER_PLAN_PRIVATE     = 9,
    E_SMSTS_NUMBER_PLAN_ERMES       = 10,
    E_SMSTS_NUMBER_PLAN_RESERVED_11 = 11,
    E_SMSTS_NUMBER_PLAN_RESERVED_12 = 12,
    E_SMSTS_NUMBER_PLAN_RESERVED_13 = 13,
    E_SMSTS_NUMBER_PLAN_RESERVED_14 = 14,
    E_SMSTS_NUMBER_PLAN_RESERVED_15 = 15,
    E_SMSTS_NUMBER_PLAN_MAX,
} e_smsts_number_plan_enum;

typedef struct smsts_address_s {
    e_smsts_digit_mode_enum digit_mode;
    e_smsts_number_type_enum number_type;
    e_smsts_number_plan_enum number_plan;

    uint8 addr_len;
    uint8 addr[SMSTS_ADDRESS_MAX_LEN];
} smsts_address_s_type;


typedef enum {
    E_SMSTS_PID_DEFAULT = 0x00,
    E_SMSTS_PID_MAX,
} e_smsts_pid_enum;


typedef enum {
    E_SMSTS_MESSAGE_CLASS_0 = 0,
    E_SMSTS_MESSAGE_CLASS_1 = 1,
    E_SMSTS_MESSAGE_CLASS_2 = 2,
    E_SMSTS_MESSAGE_CLASS_3 = 3,
    E_SMSTS_MESSAGE_CLASS_MAX,
} e_smsts_message_class_enum;

typedef enum {
    E_SMSTS_ALPHABET_7_BIT_DEFAULT = 0,
    E_SMSTS_ALPHABET_8_BIT         = 1,
    E_SMSTS_ALPHABET_UCS2          = 2,
    E_SMSTS_ALPHABET_RESERVED      = 3,
    E_SMSTS_ALPHABET_MAX,
} e_smsts_alphabet_enum;

typedef enum {
    E_SMSTS_DCS_GROUP_GENERAL = 0,
    E_SMSTS_DCS_GROUP_AUTOMATIC_DELETION = 1,
    E_SMSTS_DCS_GROUP_MESSAGE_WAITING_DISCARD = 2,
    E_SMSTS_DCS_GROUP_MESSAGE_WAITING_STORE = 3,
    E_SMSTS_DCS_GROUP_NONE_1111 = 4,
    E_SMSTS_DCS_GROUP_RESERVED = 5,
    E_SMSTS_DCS_GROUP_MAX,
} e_smsts_dsc_group_enum;

typedef struct smsts_dcs_s {
    /* Just support group general, 3GPP 23.038 */
    e_smsts_dsc_group_enum dcs_group;
    e_smsts_alphabet_enum alphabet;
} smsts_dcs_s_type;

typedef enum {
    E_SMSTS_VALIDITY_NONE = 0,
    E_SMSTS_VALIDITY_ENHANCED = 1,
    E_SMSTS_VALIDITY_RELATIVE = 2,
    E_SMSTS_VALIDITY_ABSOLUTE = 3,
    E_SMSTS_VALIDITY_MAX = 4,
} e_smsts_validity_format_enum;

typedef struct smsts_validity_s {
    uint8 year;
    uint8 month;
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;
    uint8 time_zone;
} smsts_validity_s_type;

typedef struct smsts_gw_user_data_s {
    uint32 sm_len;
    uint8 sm_data[SMSTS_USER_DATA_MAX_LEN];
} smsts_gw_user_data_s_type;

typedef struct smsts_gw_submit_s {
    uint8 message_type_indicator;                         /* TP-MTI */
    boolean reject_duplicates;                            /* TP-RD */
    e_smsts_validity_format_enum validity_period_format;  /* TP-VPF */
    boolean reply_path_present;                           /* TP-RP */
    boolean user_data_header_present;                     /* TP-UDHI */
    boolean status_report_enabled;                        /* TP-SRR */
    uint8 message_reference;                             /* TP-MR */
    smsts_address_s_type address;                         /* TP-DA */
    e_smsts_pid_enum pid;                                 /* TP-PID */
    smsts_dcs_s_type dcs;                                 /* TP-DCS */
    smsts_validity_s_type validity;                       /* TP-VP */
    smsts_gw_user_data_s_type user_data;                  /* TP-UDL & TP-UD*/
} smsts_gw_submit_s_type;

#endif  /* _SMS_TS_H_ */