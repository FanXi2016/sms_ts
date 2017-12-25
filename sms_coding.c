
#include "wms.h"
#include "wmsts.h"

#include "sms_err.h"
#include "sms_log.h"
#include "sms_coding.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "SMS"

#define SMS_DEBUG_MODE

#ifdef SMS_DEBUG_MODE
static void _print_pdu(uint8 *pdu, uint32 pdu_len)
{
    uint32 pdu_idx = 0;

    SMS_ERR("DG, pdu_len=%d\n", pdu_len);
    for (pdu_idx = 0; pdu_idx < pdu_len; pdu_idx++){
        SMS_ERR("DG, pdu[%d]=0x%X\n", pdu_idx, *(pdu + pdu_idx));
    }
}

static void _print_sms(sms_message_s_type *sms)
{
#if 0
    SMS_ERR("DG, smsc = %s.\n", sms->smsc);
    SMS_ERR("DG, sender = %s.\n", sms->number);
    SMS_ERR("DG, year = %d, month = %d, day = %d.\n", sms->validity.year, sms->validity.month, sms->validity.date);
    SMS_ERR("DG, hour = %d, min = %d, sec = %d.\n", sms->validity.hour, sms->validity.min, sms->validity.sec);
    SMS_ERR("DG, time_zone = %d.\n", sms->validity.timezone);
    SMS_ERR("DG, content_dcs = %d.\n", sms->content_dcs);
    
    SMS_ERR("DG, content_len = %d.\n", sms->content_len);
    SMS_ERR("DG, content = %s.\n", sms->content);
#else
    int index = 0;

    for (index = 0; index < strlen(sms->smsc); index++) {
        SMS_ERR("DG, smsc[%d] = 0x%X.\n", index, sms->smsc[index]);
    }
    SMS_ERR("DG, smsc = %s.\n", sms->smsc);

    for (index = 0; index < strlen(sms->telnumber); index++) {
        SMS_ERR("DG, telnumber[%d] = 0x%X.\n", index, sms->telnumber[index]);
    }    
    SMS_ERR("DG, sender = %s.\n", sms->telnumber);

    SMS_ERR("DG, year = %d, month = %d, day = %d.\n", sms->validity.year, sms->validity.month, sms->validity.date);
    SMS_ERR("DG, hour = %d, min = %d, sec = %d.\n", sms->validity.hour, sms->validity.min, sms->validity.sec);
    SMS_ERR("DG, time_zone = %d.\n", sms->validity.timezone);
    SMS_ERR("DG, content_dcs = %d.\n", sms->content_dcs);

    SMS_ERR("DG, content_len = %d.\n", sms->content_len);
    for (index = 0; index < sms->content_len; index++) {
        SMS_ERR("DG, content[index] = 0x%X.\n", index, sms->content[index]);
    }
    SMS_ERR("DG, content = %s.\n", sms->content);

#endif
}
#endif


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
        return SMS_ERR_NULL;
    }

    str_len = strlen(string);
    if (str_len == 0) {
        #ifndef SMS_LOG_TAG
        SMS_ERR("Err, String is non all digit.");
        #else
        SMS_ERR(LOG_TAG,"Err, String length is empty.");
        #endif
        return SMS_ERR_LENGTH;
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
        return SMS_ERR_NULL;
    }

    if (strlen(str_addr) == 0) {
        #ifndef SMS_LOG_TAG
        SMS_ERR("Err, Address length is empty.");
        #else
        SMS_ERR(LOG_TAG,"Err, Address length is empty.");
        #endif
        return SMS_ERR_LENGTH;
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
    if (rc == SMS_ERR_NULL || rc == SMS_ERR_LENGTH) {
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

#if 0
/** @brief sms_coding_decode_address
 *
 *  
 *
 *  @param[in] str_addr: string format of sms destination address.
 *  @param[in] wmsts_addr: wms type format address.
 *
 *  @return[integer]: function process state, failed or success.
 */
static int sms_coding_decode_address(uint8 *pdu_addr, uint8 *decode_addr)
{
    uint8 addr_idx = 0;
    uint8 addr_length = 0;
    uint8 *addr_ptr = NULL;
    wms_address_s_type wmsts_addr;

    if (pdu_addr == NULL || decode_addr == NULL) {
        SMS_ERR("Err, Addrees or PUD address is NULL pointer.");
        return SMS_ERR_NULL;
    }

    memset(&wmsts_addr, 0, sizeof(wms_address_s_type));

    addr_ptr = pdu_addr;
    addr_length = *addr_ptr;
    if (addr_length > 24) {  /* 3GPP 23.040 9.1.2.5 - 12 octets */
        SMS_ERR("Err, Addrees is too long(%d).", addr_length);
        return SMS_ERR_LENGTH;
    } else if (addr_length <= 0) {
        SMS_ERR("Err, Addrees is empty(%d).", addr_length);
        return SMS_ERR_LENGTH;
    } else {
        /* Nothing to do. */
    }
    addr_ptr++;

    wmsts_addr.number_type = (wms_number_type_e_type)((*addr_ptr & 0x70) >> 4);
    wmsts_addr.number_plan = (wms_number_plan_e_type)(*addr_ptr & 0x0F);
    addr_ptr++;

    if (wmsts_addr.number_type == WMS_NUMBER_ALPHANUMERIC) {  /* GSM 7-bit */
        wmsts_addr.digit_mode = WMS_DIGIT_MODE_8_BIT;

        /* Need Coding. */
    } else {
        wmsts_addr.digit_mode = WMS_DIGIT_MODE_4_BIT;
        wmsts_addr.number_of_digits = addr_length;

        for (addr_idx = 0; addr_idx < wmsts_addr.number_of_digits; addr_idx++) {
            wmsts_addr.digits[addr_idx] = (*(addr_ptr + addr_idx) & 0x0F) + '0';
            wmsts_addr.digits[addr_idx + 1] = ((*(addr_ptr + addr_idx) & 0xF0) >> 4) + '0';
        }
    }

    if (wmsts_addr.number_type == WMS_NUMBER_INTERNATIONAL) {  /* International number, add '+' in number front. */
        *decode_addr = '+';
        memcpy((decode_addr + 1), &wmsts_addr.digits, wmsts_addr.number_of_digits);
    } else {
        memcpy(decode_addr, &wmsts_addr.digits, wmsts_addr.number_of_digits);
    }

    return SMS_SUCCESS;
}
#endif

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

/** @brief sms_coding_decode_gw_dcs
 *
 *  decode sms content data coding scheme.
 *
 *  @param[in] wmsts_dcs: gw wms format of data coding scheme(dcs).
 *  @param[in] sms_dcs: sms content data coding scheme.
 *
 *  @return[none]
 */
static void sms_coding_decode_gw_dcs(wms_gw_dcs_s_type *wmsts_dcs, e_sms_content_dcs_enum *sms_dcs)
{
    switch (wmsts_dcs->alphabet)
    {
        case WMS_GW_ALPHABET_7_BIT_DEFAULT:
        {
            *sms_dcs = E_SMS_CONTENT_DCS_GSM_7BIT;
            break;
        }
        case WMS_GW_ALPHABET_8_BIT:
        {
            *sms_dcs = E_SMS_CONTENT_DCS_8BIT;
            break;
        }
        case WMS_GW_ALPHABET_UCS2:
        {
            *sms_dcs = E_SMS_CONTENT_DCS_USC2;
            break;
        }
        default:
        {
            *sms_dcs = E_SMS_CONTENT_DCS_UNKNOWN;
            break;
        }
    }
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

/** @brief sms_coding_decode_gw_validity_period
 *
 *  decode sms validity period.
 *
 *  @param[in] pdu_validity: gw wms format of validity period.
 *  @param[in] decode_validity: sms validity period format to show.
 *
 *  @return[none]
 */
static void sms_coding_decode_gw_validity_period(wms_timestamp_s_type *pdu_validity, sms_validity_period_s_type *decode_validity)
{
    decode_validity->year = pdu_validity->year;
    decode_validity->month = pdu_validity->month;
    decode_validity->date = pdu_validity->day;
    decode_validity->hour = pdu_validity->hour;
    decode_validity->min = pdu_validity->minute;
    decode_validity->sec = pdu_validity->second;
    decode_validity->timezone = pdu_validity->timezone;
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
        return SMS_ERR_NULL;
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

#ifdef SMS_DEBUG_MODE
    SMS_ERR("=====SUBMIT=====\n");
    _print_pdu(&wmsts_response_data.data, wmsts_response_data.len);
#endif


    if (wmsts_response_data.len > 0) {
        pdu->message_len = wmsts_response_data.len;
        memcpy(pdu->message, wmsts_response_data.data, wmsts_response_data.len);
    }
    return SMS_SUCCESS;
}



static uint8 sms_coding_decode_smsc(uint8 *pdu_message, uint8 *smsc)
{
    wms_address_s_type wmsts_addr;
    uint8 *pdu_ptr = NULL;
    uint8 *smsc_ptr = NULL;
    uint32 idx = 0;
    uint8 shift = 0;
    uint8 ch_high = 0;
    uint8 ch_low = 0;

    if (pdu_message == NULL || smsc == NULL) {
        SMS_ERR("Err,  PDU Message or SMSC is NULL pointer.");
        return SMS_ERR_NULL;
    }
    pdu_ptr = pdu_message;
    smsc_ptr = smsc;

    if (*pdu_ptr > 0) {
        memset(&wmsts_addr, 0, sizeof(wms_address_s_type));
        shift = wms_ts_decode_address(pdu_ptr, &wmsts_addr);
        if (shift == 0) {
            SMS_ERR("Err, SMSC number is too long.");
            return 0;
        }
    } else if (*pdu_ptr == 0) {
        shift = 1;
    } else {
        SMS_ERR("Err, SMSC number length.");
        return 0;
    }

    /* Convert Number to String. */
    if (wmsts_addr.number_type == WMS_NUMBER_INTERNATIONAL) {  /* International number, add '+' in number front. */
        *smsc_ptr = 0x2B;  /* '+' = 0x2B */
        smsc_ptr++;
    }

    if (wmsts_addr.digit_mode == WMS_DIGIT_MODE_8_BIT) {  /* GSM 7-bit */
        /* Need Coding. */
    } else {  /* 4-Bit */
        SMS_ERR("Info, number_of_digits = %d.", wmsts_addr.number_of_digits);
        for (idx = 0; idx < wmsts_addr.number_of_digits; idx++) {
            SMS_ERR("Info, digits[%d] = 0x%2X.", idx, wmsts_addr.digits[idx]);
        }

        for (idx = 0; idx < (wmsts_addr.number_of_digits - 1); idx++) {
            ch_low = (wmsts_addr.digits[idx] & 0x0F);
            ch_high = ((wmsts_addr.digits[idx] & 0xF0) >> 4);
#if 0
            if (ch > 0 && ch < 9) {
                *(smsc_ptr + idx) = ch + 0x30;  /* '0' = 0x30 */
            }

            ch = ((wmsts_addr.digits[idx] & 0xF0) >> 4);
#endif
            if ((wmsts_addr.digits[idx] > 0x00) && (wmsts_addr.digits[idx] < 0x09)) {
                *(smsc_ptr + idx) = wmsts_addr.digits[idx] + 0x30;  /* '0' = 0x30 */
            }
        }
    }

    return shift;
}

static uint8 sms_coding_decode_telnumber(wms_address_s_type *wmsts_addr, uint8 *telnumber)
{
    uint8 *num_ptr = NULL;
    uint32 idx = 0;

    if (wmsts_addr == NULL || telnumber == NULL) {
        SMS_ERR("Err,  wmsts_addr or Tel-Number is NULL pointer.");
        return SMS_ERR_NULL;
    }
    num_ptr = telnumber;

    /* Convert Number to String. */
    if (wmsts_addr->number_type == WMS_NUMBER_INTERNATIONAL) {  /* International number, add '+' in number front. */
        *num_ptr = 0x2B;  /* '+' = 0x2B */
        num_ptr++;
    }

    if (wmsts_addr->digit_mode == WMS_DIGIT_MODE_8_BIT) {  /* GSM 7-bit */
        /* Need Coding. */
    } else {
        for (idx = 0; idx < wmsts_addr->number_of_digits; idx++) {
            if ((wmsts_addr->digits[idx] > 0x00) && (wmsts_addr->digits[idx] < 0x09)) {
                *(num_ptr + idx) = wmsts_addr->digits[idx] + 0x30;  /* '0' = 0x30 */
            }
        }
    }

    return SMS_SUCCESS;
}



/** @brief sms_decode_pdu_deliver
 *
 *  dncode pdu messsage to sms message format, 3GPP 23.040 [9.2].
 *
 *  @param[in] pdu: Raw PDU message.
 *  @param[in] pdu_len: PDU message length.
 *  @param[out] sms_message: SMS message.
 *
 *  @return[int]: function state, success or failed.
 */
int sms_decode_pdu_deliver(uint8 *pdu, uint32 pdu_len, sms_message_s_type *sms_message)
{
    wms_raw_ts_data_s_type wmsts_raw;
    wms_client_ts_data_s_type wmsts_response;
    uint8 *pdu_ptr = NULL;
    uint8 shift = 0;
    int rc = SMS_SUCCESS;

    if (pdu == NULL || sms_message == NULL) {
        return SMS_ERR_NULL;
    }

    if (pdu_len <= 0) {
        return SMS_ERR_LENGTH;
    }

#ifdef SMS_DEBUG_MODE
    SMS_ERR("=====DELIVER=====\n");
    _print_pdu(pdu, pdu_len);
#endif

    pdu_ptr = pdu;

#if 0
    /* SMSC - SMS Center Number. */
    if (((int)*pdu_ptr) > 0) {
        smsc_len = ((int)*pdu_ptr);
        rc = sms_coding_decode_address(pdu_ptr, &sms_message->smsc);
        if (rc != SMS_SUCCESS) {
            SMS_ERR("Err, Decode SMSC number.");
            return SMS_ERR_DELIVER;
        }

        pdu_ptr = pdu_ptr + (smsc_len + 1);  /* Pointer to First octet of SMS-DELIVER message. */
    } else if (*pdu_ptr == 0) {
        pdu_ptr = pdu_ptr + 1;
        SMS_INFO("Info, SMSC number is empty.");
    } else {
        SMS_ERR("Err, SMSC number length.");
        return SMS_ERR_LENGTH;
    }
#else
    shift = sms_coding_decode_smsc(pdu_ptr, &sms_message->smsc);
    SMS_ERR("Info, shift = %d.", shift);
    if (shift == 0) {
        SMS_ERR("Err, Decode SMSC number.");
        return SMS_ERR_DELIVER;
    } else {
        pdu_ptr = pdu + shift;
    }
#endif

    memset(&wmsts_raw, 0, sizeof(wms_raw_ts_data_s_type));
    memset(&wmsts_response, 0, sizeof(wms_client_ts_data_s_type));

    wmsts_raw.format = WMS_FORMAT_GW_PP;
    wmsts_raw.tpdu_type = WMS_TPDU_DELIVER;
    wmsts_raw.len = pdu_len - shift;
    memcpy(&wmsts_raw.data, pdu_ptr, wmsts_raw.len);

    rc = wms_ts_decode(&wmsts_raw, &wmsts_response);
    if (rc != WMS_OK_S) {
        SMS_ERR("Err, Decode pdu message to WMS TS format.");
        return SMS_ERR_DELIVER;
    }

    /* SMS Sender Number. */
#if 0
    if (wmsts_response.u.gw_pp.u.deliver.address.number_type == WMS_NUMBER_INTERNATIONAL) {
        *sms_message->number = '+';
        memcpy((&sms_message->number + 1), &wmsts_response.u.gw_pp.u.deliver.address.digits, wmsts_response.u.gw_pp.u.deliver.address.number_of_digits);
    } else {
        memcpy(&sms_message->number, &wmsts_response.u.gw_pp.u.deliver.address.digits, wmsts_response.u.gw_pp.u.deliver.address.number_of_digits);
    }
#else
    rc = sms_coding_decode_telnumber(&wmsts_response.u.gw_pp.u.deliver.address, &sms_message->telnumber);
    if (rc != SMS_SUCCESS) {
        SMS_ERR("Err, Decode Tel-Number.");
        return rc;
    }
#endif

    /* SMS Validity Period. */
    sms_coding_decode_gw_validity_period(&wmsts_response.u.gw_pp.u.deliver.timestamp, &sms_message->validity);

    /* SMS Content DCS. */
    sms_coding_decode_gw_dcs(&wmsts_response.u.gw_pp.u.deliver.dcs, &sms_message->content_dcs);

    /* SMS Content. */
    sms_message->content_len = wmsts_response.u.gw_pp.u.deliver.user_data.sm_len;
    memcpy(&sms_message->content, &wmsts_response.u.gw_pp.u.deliver.user_data.sm_data, sms_message->content_len);


#ifdef SMS_DEBUG_MODE
    SMS_ERR("=====DELIVER=====\n");
    _print_sms(sms_message);
#endif

    return SMS_SUCCESS;
}
