#include <furi_hal_info.h>
#include <furi_hal_region.h>
#include <furi_hal_version.h>
#include <furi_hal_bt.h>
#include <furi_hal_crypto.h>

#include <interface/patterns/ble_thread/shci/shci.h>
#include <furi.h>
#include <protobuf_version.h>

typedef struct {
    FuriString* key;
    FuriString* value;
    FuriHalInfoValueCallback out;
    char sep;
    bool last;
    void* context;
} FuriHalInfoValueContext;

static void furi_hal_info_do_out(FuriHalInfoValueContext* ctx, const char* fmt, unsigned int nparts, ...) {
    furi_string_reset(ctx->key);

    va_list args;
    va_start(args, nparts);

    for(size_t i = 0; i < nparts; ++i) {
        const char* keypart = va_arg(args, const char*);
        furi_string_cat(ctx->key, keypart);
        if(i < nparts - 1) {
            furi_string_push_back(ctx->key, ctx->sep);
        }
    }

    const char* value_str;

    if(fmt) {
        furi_string_vprintf(ctx->value, fmt, args);
        value_str = furi_string_get_cstr(ctx->value);
     } else {
        value_str = va_arg(args, const char*);
    }

    va_end(args);

    ctx->out(furi_string_get_cstr(ctx->key), value_str, false, ctx->context);
}

void furi_hal_info_get(FuriHalInfoValueCallback out, char sep, void* context) {
    FuriString* key = furi_string_alloc();
    FuriString* value = furi_string_alloc();

    FuriHalInfoValueContext value_context = {
        .key = key,
        .value = value,
        .out = out,
        .sep = sep,
        .context = context
    };

    // Device Info version
    if(sep == '.') {
        furi_hal_info_do_out(&value_context, NULL, 2, "format", "major", "2");
        furi_hal_info_do_out(&value_context, NULL, 2, "format", "minor", "0");
    } else {
        furi_hal_info_do_out(&value_context, NULL, 3, "device", "info", "major", "2");
        furi_hal_info_do_out(&value_context, NULL, 3, "device", "info", "minor", "0");
    }

    // Model name
    furi_hal_info_do_out(&value_context, NULL, 2, "hardware", "model", furi_hal_version_get_model_name());

    // Unique ID
    furi_string_reset(value);
    const uint8_t* uid = furi_hal_version_uid();
    for(size_t i = 0; i < furi_hal_version_uid_size(); i++) {
        furi_string_cat_printf(value, "%02X", uid[i]);
    }
    furi_hal_info_do_out(&value_context, NULL, 2, "hardware", "uid", furi_string_get_cstr(value));

    // OTP Revision
    furi_hal_info_do_out(&value_context, "%d", 3, "hardware", "otp", "ver", furi_hal_version_get_otp_version());
    furi_hal_info_do_out(&value_context, "%lu", 2, "hardware", "timestamp", furi_hal_version_get_hw_timestamp());

    // Board Revision
    furi_hal_info_do_out(&value_context, "%d", 2, "hardware", "ver", furi_hal_version_get_hw_version());
    furi_hal_info_do_out(&value_context, "%d", 2, "hardware", "target", furi_hal_version_get_hw_target());
    furi_hal_info_do_out(&value_context, "%d", 2, "hardware", "body", furi_hal_version_get_hw_body());
    furi_hal_info_do_out(&value_context, "%d", 2, "hardware", "connect", furi_hal_version_get_hw_connect());
    furi_hal_info_do_out(&value_context, "%d", 2, "hardware", "display", furi_hal_version_get_hw_display());

    // Board Personification
    furi_hal_info_do_out(&value_context, "%d", 2, "hardware", "color", furi_hal_version_get_hw_color());

    if(sep == '.') {
        furi_hal_info_do_out(&value_context, "%d", 3, "hardware", "region", "builtin", furi_hal_version_get_hw_region());
    } else {
        furi_hal_info_do_out(&value_context, "%d", 2, "hardware", "region", furi_hal_version_get_hw_region());
    }

    furi_hal_info_do_out(&value_context, NULL, 3, "hardware", "region", "provisioned", furi_hal_region_get_name());

    const char* name = furi_hal_version_get_name_ptr();
    if(name) {
        furi_hal_info_do_out(&value_context, NULL, 2, "hardware", "name", name);
    }

    // Firmware version
    const Version* firmware_version = furi_hal_version_get_firmware_version();
    if(firmware_version) {
        if(sep == '.') {
            furi_hal_info_do_out(&value_context, NULL, 3, "firmware", "commit", "hash", version_get_githash(firmware_version));
        } else {
            furi_hal_info_do_out(&value_context, NULL, 2, "firmware", "commit", version_get_githash(firmware_version));
        }

        furi_hal_info_do_out(&value_context, NULL, 3, "firmware", "commit", "dirty", version_get_dirty_flag(firmware_version) ? "true" : "false");

        if(sep == '.') {
            furi_hal_info_do_out(&value_context, NULL, 3, "firmware", "branch", "name", version_get_gitbranch(firmware_version));
        } else {
            furi_hal_info_do_out(&value_context, NULL, 2, "firmware", "branch", version_get_gitbranch(firmware_version));
        }

        furi_hal_info_do_out(&value_context, NULL, 3, "firmware", "branch", "num", version_get_gitbranchnum(firmware_version));
        furi_hal_info_do_out(&value_context, NULL, 2, "firmware", "version", version_get_version(firmware_version));
        furi_hal_info_do_out(&value_context, NULL, 3, "firmware", "build", "date", version_get_builddate(firmware_version));
        furi_hal_info_do_out(&value_context, "%d", 2, "firmware", "target", version_get_target(firmware_version));
    }

    if(furi_hal_bt_is_alive()) {
        const BleGlueC2Info* ble_c2_info = ble_glue_get_c2_info();
        furi_hal_info_do_out(&value_context, NULL, 2, "radio", "alive", "true");
        furi_hal_info_do_out(&value_context, NULL, 2, "radio", "mode", ble_c2_info->mode == BleGlueC2ModeFUS ? "FUS" : "Stack");

        // FUS Info
        furi_hal_info_do_out(&value_context, "%d", 3, "radio", "fus", "major", ble_c2_info->FusVersionMajor);
        furi_hal_info_do_out(&value_context, "%d", 3, "radio", "fus", "minor", ble_c2_info->FusVersionMinor);
        furi_hal_info_do_out(&value_context, "%d", 3, "radio", "fus", "sub", ble_c2_info->FusVersionSub);
        furi_hal_info_do_out(&value_context, "%dK", 3, "radio", "fus", "sram2b", ble_c2_info->FusMemorySizeSram2B);
        furi_hal_info_do_out(&value_context, "%dK", 3, "radio", "fus", "sram2a", ble_c2_info->FusMemorySizeSram2A);
        furi_hal_info_do_out(&value_context, "%dK", 3, "radio", "fus", "flash", ble_c2_info->FusMemorySizeFlash * 4);

        // Stack Info
        furi_hal_info_do_out(&value_context, "%d", 3, "radio", "stack", "type", ble_c2_info->StackType);
        furi_hal_info_do_out(&value_context, "%d", 3, "radio", "stack", "major", ble_c2_info->VersionMajor);
        furi_hal_info_do_out(&value_context, "%d", 3, "radio", "stack", "minor", ble_c2_info->VersionMinor);
        furi_hal_info_do_out(&value_context, "%d", 3, "radio", "stack", "sub", ble_c2_info->VersionSub);
        furi_hal_info_do_out(&value_context, "%d", 3, "radio", "stack", "branch", ble_c2_info->VersionBranch);
        furi_hal_info_do_out(&value_context, "%d", 3, "radio", "stack", "release", ble_c2_info->VersionReleaseType);
        furi_hal_info_do_out(&value_context, "%dK", 3, "radio", "stack", "sram2b", ble_c2_info->MemorySizeSram2B);
        furi_hal_info_do_out(&value_context, "%dK", 3, "radio", "stack", "sram2a", ble_c2_info->MemorySizeSram2A);
        furi_hal_info_do_out(&value_context, "%dK", 3, "radio", "stack", "sram1", ble_c2_info->MemorySizeSram1);
        furi_hal_info_do_out(&value_context, "%dK", 3, "radio", "stack", "flash", ble_c2_info->MemorySizeFlash * 4);

        // Mac address
        furi_string_reset(value);
        const uint8_t* ble_mac = furi_hal_version_get_ble_mac();
        for(size_t i = 0; i < 6; i++) {
            furi_string_cat_printf(value, "%02X", ble_mac[i]);
        }
        furi_hal_info_do_out(&value_context, NULL, 3, "radio", "ble", "mac", furi_string_get_cstr(value));

        // Signature verification
        uint8_t enclave_keys = 0;
        uint8_t enclave_valid_keys = 0;
        bool enclave_valid = furi_hal_crypto_verify_enclave(&enclave_keys, &enclave_valid_keys);
        if(sep == '.') {
            furi_hal_info_do_out(&value_context, "%d", 3, "enclave", "keys", "valid", enclave_valid_keys);
        } else {
            furi_hal_info_do_out(&value_context, "%d", 3, "enclave", "valid", "keys", enclave_valid_keys);
        }

        furi_hal_info_do_out(&value_context, NULL, 2, "enclave", "valid", enclave_valid ? "true" : "false");
    } else {
        furi_hal_info_do_out(&value_context, NULL, 2, "radio", "alive", "false");
    }

    furi_hal_info_do_out(&value_context, "%u", 3, "protobuf", "version", "major", PROTOBUF_MAJOR_VERSION);
    furi_hal_info_do_out(&value_context, "%u", 3, "protobuf", "version", "minor", PROTOBUF_MINOR_VERSION);

    furi_string_free(key);
    furi_string_free(value);
}
