#include "sms_log.h"











static e_sms_error_enum sms_ts_encode_address(uint8 *addr, uint8 *pdu)
{
    uint8 *ptr_addr = addr;
    uint8 idx = 0;

    while (*ptr_addr) {
        if (*(ptr_addr + 1)) {
            pdu[idx] = ((*(ptr_addr + 1) - 48) << 4)  | (*ptr_addr - 48);
        } else {
            pdu[idx] = 0xF0 | (*ptr_addr - 48);
        }
        idx++;
        ptr_addr = ptr_addr + 2;
    }

    return idx;
}







e_sms_error_enum sms_ts_tpdu_submit(smsts_gw_submit_s_type *sms_submit, uint8 *tpdu)
{
    uint8 pos = 0;

    if (sms_submit == NULL || tpdu == NULL) {
        return SMS_E_INVALID_PARAM;
    }

    /* TP-MTI, TP-RD, TP-VPF TP-SRR, TP-UDHI, TP-RP */
    if (sms_submit->message_type_indicator == SMSTS_MESSAGE_TYPE_SUBMIT) {
        tpdu[pos] = 0x01;  /* TP-MTI: bit1, bit0 */
    } else {
        return SMS_E_MESSAGE_TYPE_ERR;
    }

    tpdu[pos] |= sms_submit->reject_duplicates ? 0x04 : 0x00;  /* TP-RD: bit2 */

    if (sms_submit->validity_period_format >= E_SMSTS_VALIDITY_MAX) {
        return SMS_E_VALIDITY_ERR;
    } else {
        tpdu[pos] |= sms_submit->validity_period_format << 3;  /* TP-VPF: bit4, bit3 */
    }

    tpdu[pos] |= sms_submit->status_report_enabled ? 0x20 : 0x00;  /* TP-SRR: bit5 */
    tpdu[pos] |= sms_submit->user_data_header_present ? 0x40 : 0x00;  /* TP-UDHI: bit6 */
    tpdu[pos] |= sms_submit->reply_path_present ? 0x80 : 0x00;  /* TP-UDHI: bit7 */

    pos++;
    /* TP-MR */
    tpdu[pos] = (uint8)sms_submit->message_reference;

    pos++;
    /* TP-DA */
    



}
