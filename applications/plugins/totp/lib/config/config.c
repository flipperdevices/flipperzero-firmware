#include "config.h"
#include <stdlib.h>
#include "../../types/common.h"
#include "../../types/token_info.h"

#define CONFIG_FILE_PATH "/ext/apps/Misc/totp.conf"
#define CONFIG_FILE_HEADER "Flipper TOTP plugin config file"
#define CONFIG_FILE_VERSION 1

Storage* totp_open_storage() {
    return furi_record_open(RECORD_STORAGE);
}

void totp_close_storage() {
    furi_record_close(RECORD_STORAGE);
}

FlipperFormat* totp_open_config_file(Storage* storage) {
    FlipperFormat* fff_data_file = flipper_format_file_alloc(storage);

    if (storage_common_stat(storage, CONFIG_FILE_PATH, NULL) == FSE_OK) {
        if(!flipper_format_file_open_existing(fff_data_file, CONFIG_FILE_PATH)) {
            FURI_LOG_E(LOGGING_TAG, "Error opening existing file %s", CONFIG_FILE_PATH);
            totp_close_config_file(fff_data_file);
            return NULL;
        }
    } else {
        if (!flipper_format_file_open_new(fff_data_file, CONFIG_FILE_PATH)) {
            totp_close_config_file(fff_data_file);
            FURI_LOG_E(LOGGING_TAG, "Error creating new file %s", CONFIG_FILE_PATH);
            return NULL;
        }

        flipper_format_write_header_cstr(fff_data_file, CONFIG_FILE_HEADER, CONFIG_FILE_VERSION);
        float tmp_tz = 0;
        flipper_format_write_float(fff_data_file, TOTP_CONFIG_KEY_TIMEZONE, &tmp_tz, 1);
        if(!flipper_format_rewind(fff_data_file)) {
            totp_close_config_file(fff_data_file);
            FURI_LOG_E(LOGGING_TAG, "Rewind error");
            return NULL;
        }
    }
    
    return fff_data_file;
}

void totp_full_save_config_file(PluginState* const plugin_state) {
    Storage* storage = totp_open_storage();
    FlipperFormat* fff_data_file = flipper_format_file_alloc(storage);

    flipper_format_file_open_always(fff_data_file, CONFIG_FILE_PATH);
    flipper_format_write_header_cstr(fff_data_file, CONFIG_FILE_HEADER, CONFIG_FILE_VERSION);
    flipper_format_write_hex(fff_data_file, TOTP_CONFIG_KEY_BASE_IV, &plugin_state->base_iv[0], 16);
    flipper_format_write_hex(fff_data_file, TOTP_CONFIG_KEY_CRYPTO_VERIFY, plugin_state->crypto_verify_data, plugin_state->crypto_verify_data_length);
    flipper_format_write_float(fff_data_file, TOTP_CONFIG_KEY_TIMEZONE, &plugin_state->timezone_offset, 1);
    ListNode* node = plugin_state->tokens_list;
    while (node != NULL) {
        TokenInfo* token_info = node->data;
        flipper_format_write_string_cstr(fff_data_file, TOTP_CONFIG_KEY_TOKEN_NAME, token_info->name);
        flipper_format_write_hex(fff_data_file, TOTP_CONFIG_KEY_TOKEN_SECRET, token_info->token, token_info->token_length);
        node = node->next;
    } 

    totp_close_config_file(fff_data_file);
    totp_close_storage();
}

void totp_config_file_load_base(PluginState* const plugin_state) {
    Storage* storage = totp_open_storage();
    FlipperFormat* fff_data_file = totp_open_config_file(storage);

    plugin_state->timezone_offset = 0;

    string_t temp_str;
    uint32_t temp_data32;
    string_init(temp_str);

    if(!flipper_format_read_header(fff_data_file, temp_str, &temp_data32)) {
        FURI_LOG_E(LOGGING_TAG, "Missing or incorrect header");
        return;
    }

    if (!flipper_format_read_hex(fff_data_file, TOTP_CONFIG_KEY_BASE_IV, &plugin_state->base_iv[0], 16)) {
        FURI_LOG_D(LOGGING_TAG, "Missing base IV");
    }

    flipper_format_rewind(fff_data_file);

    uint32_t crypto_size;
    if (flipper_format_get_value_count(fff_data_file, TOTP_CONFIG_KEY_CRYPTO_VERIFY, &crypto_size)) {
        plugin_state->crypto_verify_data = malloc(sizeof(uint8_t) * crypto_size);
        plugin_state->crypto_verify_data_length = crypto_size;
        if (!flipper_format_read_hex(fff_data_file, TOTP_CONFIG_KEY_CRYPTO_VERIFY, plugin_state->crypto_verify_data, crypto_size)) {
            FURI_LOG_D(LOGGING_TAG, "Missing crypto verify token");
            free(plugin_state->crypto_verify_data);
            plugin_state->crypto_verify_data = NULL;
        }
    }

    flipper_format_rewind(fff_data_file);

    if (!flipper_format_read_float(fff_data_file, TOTP_CONFIG_KEY_TIMEZONE, &plugin_state->timezone_offset, 1)) {
        plugin_state->timezone_offset = 0;
        FURI_LOG_D(LOGGING_TAG, "Missing timezone offset information, defaulting to 0");
    }

    string_clear(temp_str);
    totp_close_config_file(fff_data_file);
    totp_close_storage();
}

void totp_config_file_load_tokens(PluginState* const plugin_state) {
    Storage* storage = totp_open_storage();
    FlipperFormat* fff_data_file = totp_open_config_file(storage);

    string_t temp_str;
    uint32_t temp_data32;
    string_init(temp_str);

    if(!flipper_format_read_header(fff_data_file, temp_str, &temp_data32)) {
        FURI_LOG_E(LOGGING_TAG, "Missing or incorrect header");
        return;
    }

    uint8_t index = 0;
    bool has_any_plain_secret = false;
    while (true) {
        if (!flipper_format_read_string(fff_data_file, TOTP_CONFIG_KEY_TOKEN_NAME, temp_str)) {
            break;
        }
        
        TokenInfo* tokenInfo = malloc(sizeof(TokenInfo));

        const char* temp_cstr = string_get_cstr(temp_str);
        tokenInfo->name = (char *)malloc(strlen(temp_cstr) + 1);
        strcpy(tokenInfo->name, temp_cstr);

        uint32_t secret_bytes_count;
        if (!flipper_format_get_value_count(fff_data_file, TOTP_CONFIG_KEY_TOKEN_SECRET, &secret_bytes_count)) {
            continue;
        }

        if (secret_bytes_count == 1) { // Plain secret key
            if (!flipper_format_read_string(fff_data_file, TOTP_CONFIG_KEY_TOKEN_SECRET, temp_str)) {
                continue;
            }

            temp_cstr = string_get_cstr(temp_str);
            token_info_set_secret(tokenInfo, temp_cstr, strlen(temp_cstr), &plugin_state->iv[0]);
            has_any_plain_secret = true;
            FURI_LOG_W(LOGGING_TAG, "Found token with plain secret");
        } else { // encrypted
            tokenInfo->token_length = secret_bytes_count;
            tokenInfo->token = malloc(tokenInfo->token_length);
            if (!flipper_format_read_hex(fff_data_file, TOTP_CONFIG_KEY_TOKEN_SECRET, tokenInfo->token, tokenInfo->token_length)) {
                continue;
            }
        }

        FURI_LOG_D(LOGGING_TAG, "Found token \"%s\"", tokenInfo->name);

        if (plugin_state->tokens_list == NULL) {
            plugin_state->tokens_list = list_init_head(tokenInfo);
        } else {
            list_add(plugin_state->tokens_list, tokenInfo);
        }

        index++;
    }

    plugin_state->tokens_count = index;
    plugin_state->token_list_loaded = true;

    FURI_LOG_D(LOGGING_TAG, "Found %d tokens", index);

    string_clear(temp_str);
    totp_close_config_file(fff_data_file);
    totp_close_storage();

    if (has_any_plain_secret) {
        totp_full_save_config_file(plugin_state);
    }
}

void totp_close_config_file(FlipperFormat* file) {
    if (file == NULL) return;
    flipper_format_file_close(file);
    flipper_format_free(file);
}
