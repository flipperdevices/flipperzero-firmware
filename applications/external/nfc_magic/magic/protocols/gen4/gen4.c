#include "gen4.h"
#include "core/check.h"

Gen4* gen4_alloc() {
    Gen4* instance = malloc(sizeof(Gen4));

    return instance;
}

void gen4_free(Gen4* instance) {
    furi_check(instance);

    free(instance);
}

void gen4_reset(Gen4* instance) {
    furi_check(instance);

    memset(&instance->config, 0, sizeof(Gen4Config));
    memset(&instance->revision, 0, sizeof(Gen4Revision));
}

void gen4_copy(Gen4* dest, const Gen4* source) {
    furi_check(dest);
    furi_check(source);

    memcpy(dest, source, sizeof(Gen4));
}

bool gen4_password_is_set(const Gen4Password* instance) {
    furi_check(instance);

    return (instance->bytes[0] || instance->bytes[1] || instance->bytes[2] || instance->bytes[3]);
}

void gen4_password_reset(Gen4Password* instance) {
    furi_check(instance);

    memset(instance->bytes, 0, GEN4_PASSWORD_LEN);
}

void gen4_password_copy(Gen4Password* dest, const Gen4Password* source) {
    furi_check(dest);
    furi_check(source);

    memcpy(dest->bytes, source->bytes, GEN4_PASSWORD_LEN);
}

const char* gen4_get_shadow_mode_name(Gen4ShadowMode mode) {
    switch(mode) {
    case Gen4ShadowModePreWrite:
        return "Pre-Write";
    case Gen4ShadowModeRestore:
        return "Restore";
    case Gen4ShadowModeDisabled:
        return "Disabled";
    case Gen4ShadowModeHighSpeedDisabled:
        return "Disabled (High-speed)";
    case Gen4ShadowModeSplit:
        return "Split";
    default:
        return "Unknown";
    }
}

const char* gen4_get_direct_write_mode_name(Gen4DirectWriteBlock0Mode mode) {
    switch(mode) {
    case Gen4DirectWriteBlock0ModeEnabled:
        return "Enabled";
    case Gen4DirectWriteBlock0ModeDisabled:
        return "Disabled";
    case Gen4DirectWriteBlock0ModeDefault:
        return "Default";
    default:
        return "Unknown";
    }
}

const char* gen4_get_uid_len_num(Gen4UIDLength code) {
    switch(code) {
    case Gen4UIDLengthSingle:
        return "4";
    case Gen4UIDLengthDouble:
        return "7";
    case Gen4UIDLengthTriple:
        return "10";
    default:
        return "Unknown";
    }
}

const char* gen4_get_configuration_name(const Gen4Config* config) {
    switch(config->data_parsed.protocol) {
    case Gen4ProtocolMfClassic: {
        switch(config->data_parsed.total_blocks) {
        case 255:
            return "MIFARE Classic 4K";
        case 63:
            return "MIFARE Classic 1K";
        case 19:
            return "MIFARE Classic Mini (0.3K)";
        default:
            return "Unknown";
        }
    } break;
    case Gen4ProtocolMfUltralight: {
        switch(config->data_parsed.total_blocks) {
        case 63:
            return "MIFARE Ultralight";
        case 127:
            return "NTAG 2XX";
        default:
            return "Unknown";
        }
    } break;
    default:
        return "Unknown";
        break;
    };
}
