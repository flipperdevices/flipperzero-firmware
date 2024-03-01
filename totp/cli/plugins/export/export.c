#include <lib/toolbox/args.h>
#include <flipper_application/flipper_application.h>
#include "../../../lib/polyfills/memset_s.h"
#include "../../../services/config/config.h"
#include "../../../services/crypto/crypto_facade.h"
#include "../../../ui/scene_director.h"
#include "../../cli_helpers.h"
#include "../../cli_plugin_interface.h"
#include "../../cli_shared_methods.h"

static const char* BASE32_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

static void print_as_base32(const uint8_t* data, size_t length) {
    int buffer = data[0];
    size_t next = 1;
    int bitsLeft = 8;
    while(bitsLeft > 0 || next < length) {
        if(bitsLeft < 5) {
            if(next < length) {
                buffer <<= 8;
                buffer |= data[next++] & 0xFF;
                bitsLeft += 8;
            } else {
                int pad = 5 - bitsLeft;
                buffer <<= pad;
                bitsLeft += pad; //-V1026
            }
        }
        int index = 0x1F & (buffer >> (bitsLeft - 5));
        bitsLeft -= 5;
        putchar(BASE32_ALPHABET[index]);
    }
}

static void print_uri_component(const char* data, size_t length) {
    const char* c_ptr = data;
    const char* last_ptr = data + length;
    while(c_ptr < last_ptr) {
        const char c = *c_ptr;
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
           c == '-' || c == '_') {
            putchar(c);
        } else {
            printf("%%%x", c);
        }
        c_ptr++;
    }
}

static void handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    UNUSED(args);
    UNUSED(plugin_state);
    if(!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    TOTP_CLI_PRINTF_WARNING("WARNING!\r\n");
    TOTP_CLI_PRINTF_WARNING(
        "ALL THE INFORMATION (INCL. UNENCRYPTED SECRET) ABOUT ALL THE TOKENS WILL BE EXPORTED AND PRINTED TO THE CONSOLE.\r\n");
    TOTP_CLI_PRINTF_WARNING("Confirm? [y/n]\r\n");
    fflush(stdout);
    char user_pick;
    do {
        user_pick = tolower(cli_getc(cli));
    } while(user_pick != 'y' && user_pick != 'n' && user_pick != CliSymbolAsciiCR &&
            user_pick != CliSymbolAsciiETX && user_pick != CliSymbolAsciiEsc);

    if(user_pick != 'y' && user_pick != CliSymbolAsciiCR) {
        TOTP_CLI_PRINTF_INFO("User has not confirmed\r\n");
        return;
    }

    TokenInfoIteratorContext* iterator_context =
        totp_config_get_token_iterator_context(plugin_state);
    size_t total_count = totp_token_info_iterator_get_total_count(iterator_context);

    TOTP_CLI_LOCK_UI(plugin_state);

    size_t original_index = totp_token_info_iterator_get_current_token_index(iterator_context);

    cli_nl();
    TOTP_CLI_PRINTF("# --- EXPORT LIST BEGIN ---\r\n");

    for(size_t i = 0; i < total_count; i++) {
        totp_token_info_iterator_go_to(iterator_context, i);
        const TokenInfo* token_info = totp_token_info_iterator_get_current_token(iterator_context);
        TOTP_CLI_PRINTF("otpauth://%s/", token_info_get_type_as_cstr(token_info));
        print_uri_component(
            furi_string_get_cstr(token_info->name), furi_string_size(token_info->name));
        TOTP_CLI_PRINTF("?secret=");
        size_t key_length;
        uint8_t* key = totp_crypto_decrypt(
            token_info->token,
            token_info->token_length,
            &plugin_state->crypto_settings,
            &key_length);
        print_as_base32(key, key_length);
        memset_s(key, key_length, 0, key_length);
        free(key);
        TOTP_CLI_PRINTF("&algorithm=%s", token_info_get_algo_as_cstr(token_info));
        TOTP_CLI_PRINTF("&digits=%" PRIu8, token_info->digits);
        if(token_info->type == TokenTypeHOTP) {
            TOTP_CLI_PRINTF("&counter=%" PRIu64, token_info->counter);
        } else {
            TOTP_CLI_PRINTF("&period=%" PRIu8, token_info->duration);
        }
        cli_nl();
    }

    TOTP_CLI_PRINTF("# --- EXPORT LIST END ---\r\n\r\n");

    totp_token_info_iterator_go_to(iterator_context, original_index);

    TOTP_CLI_UNLOCK_UI(plugin_state);
}

static const CliPlugin plugin = {.name = "TOTP CLI Plugin: Export", .handle = &handle};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin,
};

const FlipperAppPluginDescriptor* totp_cli_export_plugin_ep() {
    return &plugin_descriptor;
}
