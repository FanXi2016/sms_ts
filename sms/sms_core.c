
#include "sms_log.h"

#include "sms_inf.h"
#include "sms_core.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "SMS"

//#define SMS_DEBUG_MODE


/** @brief sms_coding_pdu_submit
 *
 *  encode sms data to pdu submit format, 3GPP 23.040 [9.2].
 *
 *  @param[in] pdu_message: 
 *
 *  @return
 */
static e_sms_error_enum sms_core_encode_gw_address(smsts_address_s_type *smsts_addr, uint8 *addr, uint8 addr_len)
{
    if (smsts_addr == NULL || addr == NULL) {
        return SMS_E_INVALID_PARAM;
    }

    smsts_addr->digit_mode = E_SMSTS_DIGIT_MODE_8_BIT;
    smsts_addr->number_type = E_SMSTS_NUMBER_TYPE_INTERNATIONAL;
    smsts_addr->number_plan = E_SMSTS_NUMBER_PLAN_TELEPHONY;

    smsts_addr->addr_len = addr_len;
    memcpy(smsts_addr->addr, addr, addr_len);
    return SMS_E_SUCCESS;
}

/** @brief sms_coding_pdu_submit
 *
 *  encode sms data to pdu submit format, 3GPP 23.040 [9.2].
 *
 *  @param[in] pdu_message: 
 *
 *  @return
 */
static e_sms_error_enum sms_core_encode_gw_user_data(smsts_gw_user_data_s_type *user_data, uint8 *content, uint32 content_len)
{
    if (user_data == NULL || content == NULL) {
        return SMS_E_INVALID_PARAM;
    }

    user_data->sm_len = content_len;
    memcpy(user_data->sm_data, content, content_len);
    return SMS_E_SUCCESS;
}

/** @brief sms_coding_pdu_submit
 *
 *  encode sms data to pdu submit format, 3GPP 23.040 [9.2].
 *
 *  @param[in] pdu_message: 
 *
 *  @return
 */
int sms_core_pdu_submit(sms_message_s_type *sms_message, pdu_message_s_type *pdu_message)
{
    sms_message_s_type *obj = sms_message;
    smsts_gw_submit_s_type submit;
    e_sms_error_enum err = SMS_E_SUCCESS;

    if (!obj) {
        SMS_ERR("Err, sms message is NULL pointer.");
        return SMS_E_INVALID_PARAM;
    }

    memset(&submit, 0, sizeof(smsts_gw_submit_s_type));
    memset(&pdu_message, 0, sizeof(pdu_message_s_type));

    /* TP-MTI */
    submit.message_type_indicator = SMSTS_MESSAGE_TYPE_SUBMIT;
    /* TP-RD */
    submit.reject_duplicates = 0;
    /* TP-VPF */
    submit.validity_period_format = E_SMSTS_VALIDITY_RELATIVE;
    /* TP-RP */
    submit.reply_path_present = 0;
    /* TP-UDHI */
    submit.user_data_header_present = 0;
    /* TP-SRR */
    submit.status_report_enabled = 0;

    /* TP-MR */
    submit.message_reference = 0;

    /* TP-DA */
    err = sms_core_encode_gw_address(&submit.address, obj->address, obj->addr_len);
    if (err != SMS_E_SUCCESS) {
        return err;
    }

    /* TP-PID */
    submit.pid = E_SMSTS_PID_DEFAULT;

    /* TP-DCS */
    submit.dcs.dcs_group = E_SMSTS_DCS_GROUP_GENERAL;
    submit.dcs.alphabet = E_SMSTS_ALPHABET_UCS2;

    /* TP-VP */
    submit.validity.year = 0;
    submit.validity.month = 0;
    submit.validity.day = 4;
    submit.validity.hour = 0;
    submit.validity.minute = 0;
    submit.validity.second = 0;
    submit.validity.time_zone = 0;

    /* TP-UDL & TP-UD */
    err = sms_core_encode_gw_user_data(&submit.user_data, obj->content, obj->content_len);
    if (err != SMS_E_SUCCESS) {
        return err;
    }

    /* Encode to TPDU data. */
    err = sms_ts_tpdu_submit();
    if (err != SMS_E_SUCCESS) {
        return err;
    }
    return SMS_E_SUCCESS;
}
