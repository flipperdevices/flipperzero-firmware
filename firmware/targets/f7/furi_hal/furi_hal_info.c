#include <furi_hal_info.h>
#include <furi_hal_region.h>
#include <furi_hal_version.h>
#include <furi_hal_bt.h>
#include <furi_hal_crypto.h>

#include <interface/patterns/ble_thread/shci/shci.h>
#include <furi.h>
#include <protobuf_version.h>

void furi_hal_info_get(FuriHalInfoValueCallback out, char sep, void* context) {
    UNUSED(sep);
    FuriString* key = furi_string_alloc();
    FuriString* value = furi_string_alloc();

    // Device Info version
    if(sep == '.') {
        out("format.major", "2", false, context);
        out("format.minor", "0", false, context);
    } else {
        out("device_info_major", "2", false, context);
        out("device_info_minor", "0", false, context);
    }

    // Model name
    furi_string_printf(key, "%s%c%s", "hardware", sep, "model");
    out(furi_string_get_cstr(key), furi_hal_version_get_model_name(), false, context);

    // Unique ID
    furi_string_reset(value);
    const uint8_t* uid = furi_hal_version_uid();
    for(size_t i = 0; i < furi_hal_version_uid_size(); i++) {
        furi_string_cat_printf(value, "%02X", uid[i]);
    }
    furi_string_printf(key, "%s%c%s", "hardware", sep, "uid");
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);

    // OTP Revision
    furi_string_printf(key, "%s%c%s%c%s", "hardware", sep, "otp", sep, "ver");
    furi_string_printf(value, "%d", furi_hal_version_get_otp_version());
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
    furi_string_printf(key, "%s%c%s", "hardware", sep, "timestamp");
    furi_string_printf(value, "%lu", furi_hal_version_get_hw_timestamp());
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);

    // Board Revision
    furi_string_printf(key, "%s%c%s", "hardware", sep, "ver");
    furi_string_printf(value, "%d", furi_hal_version_get_hw_version());
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
    furi_string_printf(key, "%s%c%s", "hardware", sep, "target");
    furi_string_printf(value, "%d", furi_hal_version_get_hw_target());
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
    furi_string_printf(key, "%s%c%s", "hardware", sep, "body");
    furi_string_printf(value, "%d", furi_hal_version_get_hw_body());
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
    furi_string_printf(key, "%s%c%s", "hardware", sep, "connect");
    furi_string_printf(value, "%d", furi_hal_version_get_hw_connect());
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
    furi_string_printf(key, "%s%c%s", "hardware", sep, "display");
    furi_string_printf(value, "%d", furi_hal_version_get_hw_display());
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);

    // Board Personification
    furi_string_printf(key, "%s%c%s", "hardware", sep, "color");
    furi_string_printf(value, "%d", furi_hal_version_get_hw_color());
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);

    furi_string_printf(value, "%d", furi_hal_version_get_hw_region());

    if(sep == '.') {
        furi_string_printf(key, "%s.%s.%s", "hardware", "region", "builtin");
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
    } else {
        out("hardware_region", furi_string_get_cstr(value), false, context);
    }
    furi_string_printf(key, "%s%c%s%c%s", "hardware", sep, "region", sep, "provisioned");
    out(furi_string_get_cstr(key), furi_hal_region_get_name(), false, context);

    const char* name = furi_hal_version_get_name_ptr();
    if(name) {
        furi_string_printf(key, "%s%c%s", "hardware", sep, "name");
        out(furi_string_get_cstr(key), name, false, context);
    }

    // Firmware version
    const Version* firmware_version = furi_hal_version_get_firmware_version();
    if(firmware_version) {
        if(sep == '.') {
            furi_string_printf(key, "%s.%s.%s", "firmware", "commit", "hash");
            out(furi_string_get_cstr(key), version_get_githash(firmware_version), false, context);
        } else {
            out("firmware_commit", version_get_githash(firmware_version), false, context);
        }

        furi_string_printf(key, "%s%c%s%c%s", "firmware", sep, "commit", sep, "dirty");
        out(furi_string_get_cstr(key), version_get_dirty_flag(firmware_version) ? "true" : "false", false, context);

        if(sep == '.') {
            furi_string_printf(key, "%s.%s.%s", "firmware", "branch", "name");
            out(furi_string_get_cstr(key), version_get_gitbranch(firmware_version), false, context);
        } else {
            out("firmware_branch", version_get_gitbranch(firmware_version), false, context);
        }

        furi_string_printf(key, "%s%c%s%c%s", "firmware", sep, "branch", sep, "num");
        out(furi_string_get_cstr(key), version_get_gitbranchnum(firmware_version), false, context);
        furi_string_printf(key, "%s%c%s", "firmware", sep, "version");
        out(furi_string_get_cstr(key), version_get_version(firmware_version), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "firmware", sep, "build", sep, "date");
        out(furi_string_get_cstr(key), version_get_builddate(firmware_version), false, context);
        furi_string_printf(key, "%s%c%s", "firmware", sep, "target");
        furi_string_printf(value, "%d", version_get_target(firmware_version));
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
    }

    if(furi_hal_bt_is_alive()) {
        const BleGlueC2Info* ble_c2_info = ble_glue_get_c2_info();
        furi_string_printf(key, "%s%c%s", "radio", sep, "alive");
        out(furi_string_get_cstr(key), "true", false, context);
        furi_string_printf(key, "%s%c%s", "radio", sep, "mode");
        out(furi_string_get_cstr(key), ble_c2_info->mode == BleGlueC2ModeFUS ? "FUS" : "Stack", false, context);

        // FUS Info
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "fus", sep, "major");
        furi_string_printf(value, "%d", ble_c2_info->FusVersionMajor);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "fus", sep, "minor");
        furi_string_printf(value, "%d", ble_c2_info->FusVersionMinor);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "fus", sep, "sub");
        furi_string_printf(value, "%d", ble_c2_info->FusVersionSub);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "fus", sep, "sram2b");
        furi_string_printf(value, "%dK", ble_c2_info->FusMemorySizeSram2B);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "fus", sep, "sram2a");
        furi_string_printf(value, "%dK", ble_c2_info->FusMemorySizeSram2A);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "fus", sep, "flash");
        furi_string_printf(value, "%dK", ble_c2_info->FusMemorySizeFlash * 4);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);

        // Stack Info
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "stack", sep, "type");
        furi_string_printf(value, "%d", ble_c2_info->StackType);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "stack", sep, "major");
        furi_string_printf(value, "%d", ble_c2_info->VersionMajor);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "stack", sep, "minor");
        furi_string_printf(value, "%d", ble_c2_info->VersionMinor);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "stack", sep, "sub");
        furi_string_printf(value, "%d", ble_c2_info->VersionSub);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "stack", sep, "branch");
        furi_string_printf(value, "%d", ble_c2_info->VersionBranch);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "stack", sep, "release");
        furi_string_printf(value, "%d", ble_c2_info->VersionReleaseType);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "stack", sep, "sram2b");
        furi_string_printf(value, "%dK", ble_c2_info->MemorySizeSram2B);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "stack", sep, "sram2a");
        furi_string_printf(value, "%dK", ble_c2_info->MemorySizeSram2A);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "stack", sep, "sram1");
        furi_string_printf(value, "%dK", ble_c2_info->MemorySizeSram1);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
        furi_string_printf(key, "%s%c%s%c%s", "radio", sep, "stack", sep, "flash");
        furi_string_printf(value, "%dK", ble_c2_info->MemorySizeFlash * 4);
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);

        // Mac address
        furi_string_reset(value);
        const uint8_t* ble_mac = furi_hal_version_get_ble_mac();
        for(size_t i = 0; i < 6; i++) {
            furi_string_cat_printf(value, "%02X", ble_mac[i]);
        }
        furi_string_printf(key, "%s%c%s%c%s", "firmware", sep, "ble", sep, "mac");
        out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);

        // Signature verification
        uint8_t enclave_keys = 0;
        uint8_t enclave_valid_keys = 0;
        bool enclave_valid = furi_hal_crypto_verify_enclave(&enclave_keys, &enclave_valid_keys);
        furi_string_printf(value, "%d", enclave_valid_keys);
        if(sep == '.') {
            out("enclave.keys.valid", furi_string_get_cstr(value), false, context);
        } else {
            out("enclave_valid_keys", furi_string_get_cstr(value), false, context);
        }
        furi_string_printf(key, "%s%c%s", "enclave", sep, "valid");
        out(furi_string_get_cstr(key), enclave_valid ? "true" : "false", false, context);
    } else {
        furi_string_printf(key, "%s%c%s", "radio", sep, "alive");
        out(furi_string_get_cstr(key), "false", false, context);
    }

    furi_string_printf(key, "%s%c%s%c%s", "protobuf", sep, "version", sep, "major");
    furi_string_printf(value, "%u", PROTOBUF_MAJOR_VERSION);
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), false, context);
    furi_string_printf(key, "%s%c%s%c%s", "protobuf", sep, "version", sep, "minor");
    furi_string_printf(value, "%u", PROTOBUF_MINOR_VERSION);
    out(furi_string_get_cstr(key), furi_string_get_cstr(value), true, context);

    furi_string_free(key);
    furi_string_free(value);
}
