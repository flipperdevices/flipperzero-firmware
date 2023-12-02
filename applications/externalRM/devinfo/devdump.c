#include <furi.h>

#include <flipper_format.h>
#include <furi_hal_bt.h>
#include <ble_glue.h>
#include <dialogs/dialogs.h>

#define INFO_FILE_PATH EXT_PATH("device_info.txt")

int32_t devdump_app(void* p) {
    UNUSED(p);
    FURI_LOG_I("TEST", "Hello world");
    FURI_LOG_I("TEST", "I'm devdump!");

    FuriString* temp_str = furi_string_alloc();
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* info_file = flipper_format_file_alloc(storage);
    do {
        if(!flipper_format_file_open_always(info_file, INFO_FILE_PATH)) break;
        flipper_format_write_header_cstr(info_file, "Device Info", 1);

        furi_string_printf(temp_str, "%lx", DBGMCU->IDCODE);
        flipper_format_write_string(info_file, "IDCODE", temp_str);

        furi_string_printf(temp_str, "%lx", LL_FLASH_GetDeviceID());
        flipper_format_write_string(info_file, "Flash_DeviceId", temp_str);

        furi_string_printf(temp_str, "%lx", LL_FLASH_GetUDN());
        flipper_format_write_string(info_file, "Flash_UDN", temp_str);

        const BleGlueC2Info* c2_info = ble_glue_get_c2_info();
        furi_string_printf(
            temp_str,
            "%d.%d.%d.%d.%d",
            c2_info->VersionMajor,
            c2_info->VersionMinor,
            c2_info->VersionSub,
            c2_info->VersionBranch,
            c2_info->VersionReleaseType);
        flipper_format_write_string(info_file, "BLE Stack", temp_str);

        flipper_format_write_hex(info_file, "BLE MAC", furi_hal_version_get_ble_mac(), 6);

        uint32_t temp = furi_hal_version_get_hw_timestamp();
        flipper_format_write_uint32(info_file, "Hardware Timestamp", &temp, 1);

        flipper_format_write_hex(
            info_file, "UID", furi_hal_version_uid(), furi_hal_version_uid_size());
    } while(0);

    furi_string_free(temp_str);
    flipper_format_free(info_file);
    furi_record_close(RECORD_STORAGE);

    DialogMessage* message = dialog_message_alloc();
    dialog_message_set_header(message, "Saved!", 63, 3, AlignCenter, AlignTop);
    dialog_message_set_text(
        message, "Please send " INFO_FILE_PATH " to developers", 0, 17, AlignLeft, AlignTop);

    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    dialog_message_show(dialogs, message);
    dialog_message_free(message);
    furi_record_close(RECORD_DIALOGS);

    return 0;
}
