


#include "wms.h"
#include "wmsts.h"

#include "sms_coding_proc.h"

static int _StringAllIsDigit(char *string)
{
    int str_len = 0;
    int count = 0;

    if (string == NULL) {
        return -1;
    }

    str_len = strlen(string);
    if (str_len == 0) {
        return count;
    }

    for (count = 0; count < str_len; count++)  {
        if (*(string + count) < '0' || *(string + count) > '9') {
            return -1;
        }
    }

    return count;
}

/** @brief sms_coding_encode_address
 *
 *  [Description]
 *
 *  @param[in]
 *
 *  @return
 */
static int sms_coding_encode_address(char *addr, wms_address_s_type *pdu_address)
{
    char *addr_ptr = NULL;
    int is_international = 0;

    if (addr == NULL || pdu_address == NULL) {
        return -1;
    }

    if (strlen(addr) == 0) {
        return 0;
    }

    if (*addr == "+") {
        is_international = 1;
        addr_ptr = addr + 1;
    } else {
        is_international = 0;
        addr_ptr = addr;
    }

    if (_StringAllIsDigit(addr_ptr) < 1) {
        return 0;
    }

    /*  */
    if (is_international == 1) {
        pdu_address->number_type = WMS_NUMBER_INTERNATIONAL;
    } else {
        pdu_address->number_type = WMS_NUMBER_NATIONAL;
    }
    pdu_address->digit_mode = WMS_DIGIT_MODE_4_BIT;
    pdu_address->number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
    pdu_address->number_plan = WMS_NUMBER_PLAN_TELEPHONY;

    pdu_address->number_of_digits = strlen(addr_ptr);
    memcpy(pdu_address->digits, addr_ptr, strlen(addr_ptr));

    return 0;
}

/** @brief sms_coding_encode_gw_dcs
 *
 *  [Description]
 *
 *  @param[in]
 *
 *  @return
 */
static void sms_coding_encode_gw_user_data(char* user_data, uint32 user_data_len, wms_gw_user_data_s_type *pdu_user_data)
{
    //pdu_user_data->num_headers = 0;
    //pdu_user_data->headers = 0;
    pdu_user_data->sm_len = user_data_len;
    if (user_data_len >= 0) {
        memcpy(pdu_user_data->sm_data, user_data, user_data_len);
    }
}

/** @brief sms_coding_encode_gw_dcs
 *
 *  [Description]
 *
 *  @param[in]
 *
 *  @return
 */
static void sms_coding_encode_gw_dcs(wms_gw_alphabet_e_type sms_alphabet, wms_gw_dcs_s_type *pdu_dcs)
{
    pdu_dcs->alphabet = sms_alphabet;
    pdu_dcs->msg_class = WMS_MESSAGE_CLASS_NONE;
    pdu_dcs->is_compressed = FALSE;
    pdu_dcs->msg_waiting = WMS_GW_MSG_WAITING_NONE;
    pdu_dcs->msg_waiting_active = FALSE;
    pdu_dcs->msg_waiting_kind = WMS_GW_MSG_WAITING_VOICEMAIL;
}

/** @brief sms_coding_encode_gw_validity_period
 *
 *  [Description]
 *
 *  @param[in]
 *
 *  @return
 */
static void sms_coding_encode_gw_validity_period(wms_gw_validity_s_type *validity_period)
{
    validity_period->format = WMS_GW_VALIDITY_RELATIVE;
    validity_period->u.time.year = 0;  /* 00 - 99 */
    validity_period->u.time.month = 0;  /* 01 - 12 */
    validity_period->u.time.day = 0;  /* 01 - 31 */
    validity_period->u.time.hour = 4;  /* 00 - 23 */
    validity_period->u.time.minute = 0;  /* 00 - 59 */
    validity_period->u.time.second = 0;  /* 00 - 59 */
    validity_period->u.time.timezone = 0;
}

/** @brief sms_coding_pdu_submit
 *
 *  [Description]
 *
 *  @param[in]
 *
 *  @return
 */
int sms_coding_pdu_submit(char *dest_address, char *user_data, uint32 user_data_len, sms_coding_pdu_message_s_type *pdu)
{
    wms_client_ts_data_s_type wmsts_client;
    wms_raw_ts_data_s_type wmsts_response_data;

    if (dest_address == NULL || user_data == NULL || pdu == NULL) {
        return 0;
    }

    memset(&wmsts_client, 0, sizeof(wms_client_ts_data_s_type));
    memset(&wmsts_response_data, 0, sizeof(wms_raw_ts_data_s_type));

    wmsts_client.format = WMS_FORMAT_GW_PP;

    /* Config PDU Format - 3GPP 23.040 [9.2] */
    /* TP-MTI */
    wmsts_client.u.gw_pp.tpdu_type = WMS_TPDU_SUBMIT;

    /* TP-RD */
    wmsts_client.u.gw_pp.u.submit.reject_duplicates = 0;

    /* TP-RP */
    wmsts_client.u.gw_pp.u.submit.reply_path_present = 0;

    /* TP-UDHI */
    wmsts_client.u.gw_pp.u.submit.user_data_header_present = 0;

    /* TP-SRR */
    wmsts_client.u.gw_pp.u.submit.status_report_enabled = 0;

    /* TP-MR */
    wmsts_client.u.gw_pp.u.submit.message_reference = 0;

    /* TP-DA */
    sms_coding_encode_address(dest_address, &wmsts_client.u.gw_pp.u.submit.address);

    /* TP-PID */
    wmsts_client.u.gw_pp.u.submit.pid = WMS_PID_DEFAULT;

    /* TP-DCS */
    sms_coding_encode_gw_dcs(WMS_GW_ALPHABET_UCS2, &wmsts_client.u.gw_pp.u.submit.dcs);

    /* TP-VPF & TP-VP */
    sms_coding_encode_gw_validity_period(&wmsts_client.u.gw_pp.u.submit.validity.u.time);

    /* TP-UDL & TP-UD */
    sms_coding_encode_gw_user_data(user_data, user_data_len, &wmsts_client.u.gw_pp.u.submit.user_data);

    /* encode PDU data. */
    wms_ts_encode(&wmsts_client, &wmsts_response_data);

#if 1
    int index = 0;
    for (index; index < wmsts_response_data.len; index++) {
        printf("pdu[%d]=0x%x.\n", index, wmsts_response_data.data[index]);
    }
#endif
    return 0;
}
