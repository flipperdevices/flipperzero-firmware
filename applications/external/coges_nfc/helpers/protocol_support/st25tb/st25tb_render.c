#include "st25tb_render.h"
#include <nfc/protocols/st25tb/st25tb.h>
#include "mykey.h"

void nfc_render_st25tb_info(
    const St25tbData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    furi_string_cat_printf(str, "ID:");

    for(int i = ST25TB_UID_SIZE - 1; i >= 0; i--) {
        furi_string_cat_printf(str, " %02X", data->uid[i]);
    }

    uint32_t _uid = get_uid(data->uid);
    uint32_t _count_down_counter = get_count_down_counter(&data->blocks[6]);
    uint32_t _vendor_id =
        get_vendor(data->blocks[MYKEY_BLOCK_VENDOR_ID_1], data->blocks[MYKEY_BLOCK_VENDOR_ID_2]);
    uint32_t _master_key = get_master_key(_uid, _vendor_id);
    uint32_t _encryption_key = get_encryption_key(_master_key, _count_down_counter);
    uint16_t credit = get_xored_block(data->blocks[MYKEY_BLOCK_CURRENT_CREDIT], _encryption_key);
    uint16_t _previous_credit = get_block(data->blocks[MYKEY_BLOCK_PREVIOUS_CREDIT]);
    bool _is_bound = get_is_bound(_vendor_id);
    furi_string_cat_printf(str, "\nCurrent Credit: %d.%02d E", credit / 100, credit % 100);
    furi_string_cat_printf(
        str, "\nPrevius Credit: %d.%02d E", _previous_credit / 100, _previous_credit % 100);
    furi_string_cat_printf(str, "\nIs Bound: %s", _is_bound ? "Yes" : "No");

    if(format_type == NfcProtocolFormatTypeFull) {
        //info data
        furi_string_cat_printf(str, "\nUID: %08lX\n", _uid);
        furi_string_cat_printf(str, "ID: %08lX\n", st25tb_get_block_value(data->blocks[7]));
        furi_string_cat_printf(str, "Counter: %08lX", _count_down_counter);
        furi_string_cat_printf(str, "\nVendor ID: %08lX", _vendor_id);
        furi_string_cat_printf(str, "\nMaster Key: %08lX", _master_key);
        furi_string_cat_printf(str, "\nEncryption Key: %08lX", _encryption_key);
        furi_string_cat_printf(str, "\nBlocks:");
        for(size_t i = 0; i < st25tb_get_block_count(data->type); i += 2) {
            furi_string_cat_printf(
                str,
                "\n %02X   %08lX  %08lX",
                i,
                st25tb_get_block_value(data->blocks[i]),
                st25tb_get_block_value(data->blocks[i + 1]));
        }
    }
}
