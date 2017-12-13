#include "sms_err.h"
#include "sms_log.h"

#include "wms.h"
#include "wmsts.h"
#include "sms_coding_proc.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "SMS"


static int _StringAllIsDigit(char *string)
{
    int str_len = 0;
    int count = 0;

    if (string == NULL) {
        #ifndef SMS_LOG_TAG
        SMS_ERR("Err, String is NULL pointer.");
        #else
        SMS_ERR(LOG_TAG,"Err, String is NULL pointer.");
        #endif
        return SMS_ERR_POINTER_NULL;
    }

    str_len = strlen(string);
    if (str_len == 0) {
        #ifndef SMS_LOG_TAG
        SMS_ERR("Err, String is non all digit.");
        #else
        SMS_ERR(LOG_TAG,"Err, String length is empty.");
        #endif
        return SMS_ERR_ADDR_LEN_EMPTY;
    }

    for (count = 0; count < str_len; count++)  {
        if (*(string + count) < '0' || *(string + count) > '9') {
            #ifndef SMS_LOG_TAG
            SMS_ERR("Err, String is non all digit.");
            #else
            SMS_ERR(LOG_TAG,"Err, String is non all digit.");
            #endif
            return SMS_ERR_ADDR_NON_DIGIT;
        }
    }
    return count;
}

/** @brief sms_coding_encode_address
 *
 *  encode sms destination address from string format to wms address format.
 *
 *  @param[in] str_addr: string format of sms destination address.
 *  @param[in] wmsts_addr: wms type format address.
 *
 *  @return[integer]: function process state, failed or success.
 */
static int sms_coding_encode_address(char *str_addr, wms_address_s_type *wmsts_addr)
{
    int rc = SMS_SUCCESS;
    char *addr_ptr = NULL;
    int is_international = 0;

    if (str_addr == NULL || wmsts_addr == NULL) {
        #ifndef SMS_LOG_TAG
        SMS_ERR("Err, Addrees or PUD address is NULL pointer.");
        #else
        SMS_ERR(LOG_TAG,"Err, Addrees or PUD address is NULL pointer.");
        #endif
        return SMS_ERR_POINTER_NULL;
    }

    if (strlen(str_addr) == 0) {
        #ifndef SMS_LOG_TAG
        SMS_ERR("Err, Address length is empty.");
        #else
        SMS_ERR(LOG_TAG,"Err, Address length is empty.");
        #endif
        return SMS_ERR_ADDR_LEN_EMPTY;
    }

    /* SMS address is international number of not. */
    if (*str_addr == "+") {
        is_international = 1;
        addr_ptr = str_addr + 1;
    } else {
        is_international = 0;
        addr_ptr = str_addr;
    }

    /* Check sms address, if it all of digit. */
    rc = _StringAllIsDigit(str_addr);
    if (rc == SMS_ERR_POINTER_NULL || rc == SMS_ERR_ADDR_LEN_EMPTY) {
        return rc;
    }

    /* Encode string address to WMS format. */
    if (is_international == 1) {
        wmsts_addr->number_type = WMS_NUMBER_INTERNATIONAL;
    } else {
        wmsts_addr->number_type = WMS_NUMBER_NATIONAL;
    }
    wmsts_addr->digit_mode = WMS_DIGIT_MODE_4_BIT;
    wmsts_addr->number_mode = WMS_NUMBER_MODE_DATA_NETWORK_MAX32;
    wmsts_addr->number_plan = WMS_NUMBER_PLAN_TELEPHONY;
    wmsts_addr->number_of_digits = strlen(addr_ptr);
    memcpy(wmsts_addr->digits, addr_ptr, strlen(addr_ptr));

    return SMS_SUCCESS;
}

/** @brief sms_coding_encode_gw_dcs
 *
 *  encode sms content to gw wms format.
 *
 *  @param[in] user_data: sms content.
 *  @param[in] user_data_len: the length of sms content.
 *  @param[in] wmsts_user_data: gw wms format user data.
 *
 *  @return[none]
 */
static void sms_coding_encode_gw_user_data(char* user_data, uint32 user_data_len, wms_gw_user_data_s_type *wmsts_user_data)
{
    wmsts_user_data->sm_len = user_data_len;
    if (user_data_len >= 0) {
        memcpy(wmsts_user_data->sm_data, user_data, user_data_len);
    }
}

/** @brief sms_coding_encode_gw_dcs
 *
 *  encode sms content alphabet to gw wms format.
 *
 *  @param[in] sms_alphabet: sms content coding scheme.
 *  @param[in] wmsts_dcs: gw wms format of data coding scheme(dcs).
 *
 *  @return[none]
 */
static void sms_coding_encode_gw_dcs(wms_gw_alphabet_e_type sms_alphabet, wms_gw_dcs_s_type *wmsts_dcs)
{
    wmsts_dcs->alphabet = sms_alphabet;
    wmsts_dcs->msg_class = WMS_MESSAGE_CLASS_NONE;
    wmsts_dcs->is_compressed = FALSE;
    wmsts_dcs->msg_waiting = WMS_GW_MSG_WAITING_NONE;
    wmsts_dcs->msg_waiting_active = FALSE;
    wmsts_dcs->msg_waiting_kind = WMS_GW_MSG_WAITING_VOICEMAIL;
}

/** @brief sms_coding_encode_gw_validity_period
 *
 *  encode sms validity period.
 *
 *  @param[in] wmsts_validity_period: gw wms format of sms validity period.
 *
 *  @return[none]
 */
static void sms_coding_encode_gw_validity_period(wms_gw_validity_s_type *wmsts_validity_period)
{
    wmsts_validity_period->format = WMS_GW_VALIDITY_NONE;
    wmsts_validity_period->u.time.year = 0;  /* 00 - 99 */
    wmsts_validity_period->u.time.month = 0;  /* 01 - 12 */
    wmsts_validity_period->u.time.day = 0;  /* 01 - 31 */
    wmsts_validity_period->u.time.hour = 0;  /* 00 - 23 */
    wmsts_validity_period->u.time.minute = 0;  /* 00 - 59 */
    wmsts_validity_period->u.time.second = 0;  /* 00 - 59 */
    wmsts_validity_period->u.time.timezone = 0;
}

/** @brief sms_coding_pdu_submit
 *
 *  encode sms data to pdu submit format, 3GPP 23.040 [9.2].
 *
 *  @param[in] address: 
 *  @param[in] content: 
 *  @param[in] content_len: 
 *  @param[in] pdu: 
 *
 *  @return
 */
int sms_coding_pdu_submit(char *address, char *content, uint32 content_len, sms_coding_pdu_message_s_type *pdu)
{
    wms_client_ts_data_s_type wmsts_client;
    wms_raw_ts_data_s_type wmsts_response_data;
    int rc = SMS_SUCCESS;

    if (address == NULL || content == NULL || pdu == NULL) {
        #ifndef SMS_LOG_TAG
        SMS_ERR("Err, Addrees/Content or PDU is NULL pointer.");
        #else
        SMS_ERR(LOG_TAG,"Err, Addrees/Content or PDU is NULL pointer.");
        #endif
        return SMS_ERR_POINTER_NULL;
    }

    memset(&wmsts_client, 0, sizeof(wms_client_ts_data_s_type));
    memset(&wmsts_response_data, 0, sizeof(wms_raw_ts_data_s_type));

    /* The SMS format is GSM, not CDMA. */
    wmsts_client.format = WMS_FORMAT_GW_PP;

    /* Config PDU Submit Format. */
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
    rc = sms_coding_encode_address(address, &wmsts_client.u.gw_pp.u.submit.address);
    if (rc != SMS_SUCCESS) {
        #ifndef SMS_LOG_TAG
        SMS_ERR("Err, Encode address to WMS format.");
        #else
        SMS_ERR(LOG_TAG,"Err, Encode address to WMS format.");
        #endif
        return rc;
    }

#if 0
    SMS_ERR("address = %s.\n", address);
    SMS_ERR("digit_mode = %d.\n", wmsts_client.u.gw_pp.u.submit.address.digit_mode);
    SMS_ERR("number_mode = %d.\n", wmsts_client.u.gw_pp.u.submit.address.number_mode);
    SMS_ERR("number_type = %d.\n", wmsts_client.u.gw_pp.u.submit.address.number_type);
    SMS_ERR("number_plan = %d.\n", wmsts_client.u.gw_pp.u.submit.address.number_plan);
    SMS_ERR("number_of_digits = %d.\n", wmsts_client.u.gw_pp.u.submit.address.number_of_digits);
    SMS_ERR("digits = %s.\n", wmsts_client.u.gw_pp.u.submit.address.digits);
#endif

    /* TP-PID */
    wmsts_client.u.gw_pp.u.submit.pid = WMS_PID_DEFAULT;

    /* TP-DCS */
    //sms_coding_encode_gw_dcs(WMS_GW_ALPHABET_UCS2, &wmsts_client.u.gw_pp.u.submit.dcs);
    sms_coding_encode_gw_dcs(WMS_GW_ALPHABET_8_BIT, &wmsts_client.u.gw_pp.u.submit.dcs);

    /* TP-VPF & TP-VP */
    sms_coding_encode_gw_validity_period(&wmsts_client.u.gw_pp.u.submit.validity.u.time);

    /* TP-UDL & TP-UD */
    sms_coding_encode_gw_user_data(content, content_len, &wmsts_client.u.gw_pp.u.submit.user_data);

    /* encode PDU data. */
    rc = wms_ts_encode(&wmsts_client, &wmsts_response_data);
    if (rc != WMS_OK_S) {
        #ifndef SMS_LOG_TAG
        SMS_ERR("Err, Encode sms to PDU submit format.");
        #else
        SMS_ERR(LOG_TAG,"Err, Encode sms to PDU submit format.");
        #endif
        return SMS_ERR_SUBMIT;
    }

#if 1
#ifndef SMS_LOG_TAG
    SMS_ERR("content_len = %d, content = %s.\n", content_len, content);
#else
    SMS_ERR(LOG_TAG,"content_len = %d, content = %s.\n", content_len, content);
#endif

    int index = 0;
    for (index; index < wmsts_response_data.len; index++) {
        #ifndef SMS_LOG_TAG
        SMS_ERR("pdu[%d]=0x%2X.\n", index, wmsts_response_data.data[index]);
        #else
        SMS_ERR(LOG_TAG,"pdu[%d]=0x%2X.\n", index, wmsts_response_data.data[index]);
        #endif
    }
#endif

    if (wmsts_response_data.len > 0) {
        pdu->message_len = wmsts_response_data.len;
        memcpy(pdu->message, wmsts_response_data.data, wmsts_response_data.len);
    }
    return SMS_SUCCESS;
}
