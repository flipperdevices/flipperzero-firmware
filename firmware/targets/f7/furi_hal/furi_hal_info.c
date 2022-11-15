#include <furi_hal_info.h>
#include <furi_hal_region.h>
#include <furi_hal_version.h>
#include <furi_hal_bt.h>
#include <furi_hal_crypto.h>

#include <interface/patterns/ble_thread/shci/shci.h>
#include <furi.h>
#include <protobuf_version.h>

void furi_hal_info_get(FuriHalInfoValueCallback out, char sep, void* context) {
    FuriString* key = furi_string_alloc();
    FuriString* value = furi_string_alloc();

    RpcHelperPropertyContext property_context = {
        .key = key,
        .value = value,
        .out = out,
        .sep = sep,
        .last = false,
        .context = context
    };

    // Device Info version
    if(sep == '.') {
        rpc_helper_property_out(&property_context, NULL, 2, "format", "major", "2");
        rpc_helper_property_out(&property_context, NULL, 2, "format", "minor", "0");
    } else {
        rpc_helper_property_out(&property_context, NULL, 3, "device", "info", "major", "2");
        rpc_helper_property_out(&property_context, NULL, 3, "device", "info", "minor", "0");
    }

    // Model name
    rpc_helper_property_out(&property_context, NULL, 2, "hardware", "model", furi_hal_version_get_model_name());

    // Unique ID
    furi_string_reset(value);
    const uint8_t* uid = furi_hal_version_uid();
    for(size_t i = 0; i < furi_hal_version_uid_size(); i++) {
        furi_string_cat_printf(value, "%02X", uid[i]);
    }
    rpc_helper_property_out(&property_context, NULL, 2, "hardware", "uid", furi_string_get_cstr(value));

    // OTP Revision
    rpc_helper_property_out(&property_context, "%d", 3, "hardware", "otp", "ver", furi_hal_version_get_otp_version());
    rpc_helper_property_out(&property_context, "%lu", 2, "hardware", "timestamp", furi_hal_version_get_hw_timestamp());

    // Board Revision
    rpc_helper_property_out(&property_context, "%d", 2, "hardware", "ver", furi_hal_version_get_hw_version());
    rpc_helper_property_out(&property_context, "%d", 2, "hardware", "target", furi_hal_version_get_hw_target());
    rpc_helper_property_out(&property_context, "%d", 2, "hardware", "body", furi_hal_version_get_hw_body());
    rpc_helper_property_out(&property_context, "%d", 2, "hardware", "connect", furi_hal_version_get_hw_connect());
    rpc_helper_property_out(&property_context, "%d", 2, "hardware", "display", furi_hal_version_get_hw_display());

    // Board Personification
    rpc_helper_property_out(&property_context, "%d", 2, "hardware", "color", furi_hal_version_get_hw_color());

    if(sep == '.') {
        rpc_helper_property_out(&property_context, "%d", 3, "hardware", "region", "builtin", furi_hal_version_get_hw_region());
    } else {
        rpc_helper_property_out(&property_context, "%d", 2, "hardware", "region", furi_hal_version_get_hw_region());
    }

    rpc_helper_property_out(&property_context, NULL, 3, "hardware", "region", "provisioned", furi_hal_region_get_name());

    const char* name = furi_hal_version_get_name_ptr();
    if(name) {
        rpc_helper_property_out(&property_context, NULL, 2, "hardware", "name", name);
    }

    // Firmware version
    const Version* firmware_version = furi_hal_version_get_firmware_version();
    if(firmware_version) {
        if(sep == '.') {
            rpc_helper_property_out(&property_context, NULL, 3, "firmware", "commit", "hash", version_get_githash(firmware_version));
        } else {
            rpc_helper_property_out(&property_context, NULL, 2, "firmware", "commit", version_get_githash(firmware_version));
        }

        rpc_helper_property_out(&property_context, NULL, 3, "firmware", "commit", "dirty", version_get_dirty_flag(firmware_version) ? "true" : "false");

        if(sep == '.') {
            rpc_helper_property_out(&property_context, NULL, 3, "firmware", "branch", "name", version_get_gitbranch(firmware_version));
        } else {
            rpc_helper_property_out(&property_context, NULL, 2, "firmware", "branch", version_get_gitbranch(firmware_version));
        }

        rpc_helper_property_out(&property_context, NULL, 3, "firmware", "branch", "num", version_get_gitbranchnum(firmware_version));
        rpc_helper_property_out(&property_context, NULL, 2, "firmware", "version", version_get_version(firmware_version));
        rpc_helper_property_out(&property_context, NULL, 3, "firmware", "build", "date", version_get_builddate(firmware_version));
        rpc_helper_property_out(&property_context, "%d", 2, "firmware", "target", version_get_target(firmware_version));
    }

    if(furi_hal_bt_is_alive()) {
        const BleGlueC2Info* ble_c2_info = ble_glue_get_c2_info();
        rpc_helper_property_out(&property_context, NULL, 2, "radio", "alive", "true");
        rpc_helper_property_out(&property_context, NULL, 2, "radio", "mode", ble_c2_info->mode == BleGlueC2ModeFUS ? "FUS" : "Stack");

        // FUS Info
        rpc_helper_property_out(&property_context, "%d", 3, "radio", "fus", "major", ble_c2_info->FusVersionMajor);
        rpc_helper_property_out(&property_context, "%d", 3, "radio", "fus", "minor", ble_c2_info->FusVersionMinor);
        rpc_helper_property_out(&property_context, "%d", 3, "radio", "fus", "sub", ble_c2_info->FusVersionSub);
        rpc_helper_property_out(&property_context, "%dK", 3, "radio", "fus", "sram2b", ble_c2_info->FusMemorySizeSram2B);
        rpc_helper_property_out(&property_context, "%dK", 3, "radio", "fus", "sram2a", ble_c2_info->FusMemorySizeSram2A);
        rpc_helper_property_out(&property_context, "%dK", 3, "radio", "fus", "flash", ble_c2_info->FusMemorySizeFlash * 4);

        // Stack Info
        rpc_helper_property_out(&property_context, "%d", 3, "radio", "stack", "type", ble_c2_info->StackType);
        rpc_helper_property_out(&property_context, "%d", 3, "radio", "stack", "major", ble_c2_info->VersionMajor);
        rpc_helper_property_out(&property_context, "%d", 3, "radio", "stack", "minor", ble_c2_info->VersionMinor);
        rpc_helper_property_out(&property_context, "%d", 3, "radio", "stack", "sub", ble_c2_info->VersionSub);
        rpc_helper_property_out(&property_context, "%d", 3, "radio", "stack", "branch", ble_c2_info->VersionBranch);
        rpc_helper_property_out(&property_context, "%d", 3, "radio", "stack", "release", ble_c2_info->VersionReleaseType);
        rpc_helper_property_out(&property_context, "%dK", 3, "radio", "stack", "sram2b", ble_c2_info->MemorySizeSram2B);
        rpc_helper_property_out(&property_context, "%dK", 3, "radio", "stack", "sram2a", ble_c2_info->MemorySizeSram2A);
        rpc_helper_property_out(&property_context, "%dK", 3, "radio", "stack", "sram1", ble_c2_info->MemorySizeSram1);
        rpc_helper_property_out(&property_context, "%dK", 3, "radio", "stack", "flash", ble_c2_info->MemorySizeFlash * 4);

        // Mac address
        furi_string_reset(value);
        const uint8_t* ble_mac = furi_hal_version_get_ble_mac();
        for(size_t i = 0; i < 6; i++) {
            furi_string_cat_printf(value, "%02X", ble_mac[i]);
        }
        rpc_helper_property_out(&property_context, NULL, 3, "radio", "ble", "mac", furi_string_get_cstr(value));

        // Signature verification
        uint8_t enclave_keys = 0;
        uint8_t enclave_valid_keys = 0;
        bool enclave_valid = furi_hal_crypto_verify_enclave(&enclave_keys, &enclave_valid_keys);
        if(sep == '.') {
            rpc_helper_property_out(&property_context, "%d", 3, "enclave", "keys", "valid", enclave_valid_keys);
        } else {
            rpc_helper_property_out(&property_context, "%d", 3, "enclave", "valid", "keys", enclave_valid_keys);
        }

        rpc_helper_property_out(&property_context, NULL, 2, "enclave", "valid", enclave_valid ? "true" : "false");
    } else {
        rpc_helper_property_out(&property_context, NULL, 2, "radio", "alive", "false");
    }

    rpc_helper_property_out(&property_context, "%u", 3, "protobuf", "version", "major", PROTOBUF_MAJOR_VERSION);
    property_context.last = true;
    rpc_helper_property_out(&property_context, "%u", 3, "protobuf", "version", "minor", PROTOBUF_MINOR_VERSION);

    furi_string_free(key);
    furi_string_free(value);
}
