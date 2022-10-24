#include "add.h"
#include <stdlib.h>
#include <lib/toolbox/args.h>
#include "../../../list/list.h"
#include "../../../../types/token_info.h"
#include "../../../config/config.h"
#include "../../cli_common_helpers.h"
#include "../../../../scenes/scene_director.h"

static bool token_info_set_digits_from_str(TokenInfo* token_info, FuriString* str) {
    switch(furi_string_get_char(str, 0)) {
    case '6':
        token_info->digits = TOTP_6_DIGITS;
        return true;
    case '8':
        token_info->digits = TOTP_8_DIGITS;
        return true;
    }

    return false;
}

static bool token_info_set_algo_from_str(TokenInfo* token_info, FuriString* str) {
    if(furi_string_cmpi_str(str, TOTP_CONFIG_TOKEN_ALGO_SHA1_NAME) == 0) {
        token_info->algo = SHA1;
        return true;
    } 
    
    if(furi_string_cmpi_str(str, TOTP_CONFIG_TOKEN_ALGO_SHA256_NAME) == 0) {
        token_info->algo = SHA256;
        return true;
    } 
    
    if(furi_string_cmpi_str(str, TOTP_CONFIG_TOKEN_ALGO_SHA512_NAME) == 0) {
        token_info->algo = SHA512;
        return true;
    }

    return false;
}

void totp_cli_handle_add_command(PluginState* plugin_state, FuriString* args) {
    FuriString* temp_str = furi_string_alloc();
    const char* temp_cstr;

    TokenInfo* token_info = token_info_alloc();

    // Reading token name
    if (!args_read_probably_quoted_string_and_trim(args, temp_str)) {
        totp_cli_print_invalid_arguments();
        furi_string_free(temp_str);
        token_info_free(token_info);
        return;
    }

    temp_cstr = furi_string_get_cstr(temp_str);
    token_info->name = malloc(strlen(temp_cstr) + 1);
    strcpy(token_info->name, temp_cstr);

    // Reading token secret
    if (!args_read_probably_quoted_string_and_trim(args, temp_str)) {
        totp_cli_print_invalid_arguments();
        furi_string_free(temp_str);
        token_info_free(token_info);
        return;
    }

    temp_cstr = furi_string_get_cstr(temp_str);
    if (!token_info_set_secret(token_info, temp_cstr, strlen(temp_cstr), plugin_state->iv)) {
        printf("Token secret seems to be invalid and can not be parsed\r\n");
        furi_string_free(temp_str);
        token_info_free(token_info);
        return;
    }

    // Read optional arguments
    while (args_read_string_and_trim(args, temp_str)) {
        bool parsed = false;
        if (furi_string_cmpi_str(temp_str, "-a") == 0) {
            if (!args_read_string_and_trim(args, temp_str)) {
                printf("Missed value for argument \"-a\"\r\n");
            } else if (!token_info_set_algo_from_str(token_info, temp_str)) {
                printf("\"%s\" is incorrect value for argument \"-a\"\r\n", furi_string_get_cstr(temp_str));
            } else {
                parsed = true;
            }
        } else if (furi_string_cmpi_str(temp_str, "-d") == 0) {
            if (!args_read_string_and_trim(args, temp_str)) {
                printf("Missed value for argument \"-d\"\r\n");
            } else if (!token_info_set_digits_from_str(token_info, temp_str)) {
                printf("\"%s\" is incorrect value for argument \"-d\"\r\n", furi_string_get_cstr(temp_str));
            } else {
                parsed = true;
            }
        }
        if (!parsed) {
            totp_cli_print_invalid_arguments();
            furi_string_free(temp_str);
            token_info_free(token_info);
            return;
        }
    }

    bool load_generate_token_scene = false;
    if (plugin_state->current_scene == TotpSceneGenerateToken) {
        totp_scene_director_activate_scene(plugin_state, TotpSceneNone, NULL);
        load_generate_token_scene = true;
    }

    if(plugin_state->tokens_list == NULL) {
        plugin_state->tokens_list = list_init_head(token_info);
    } else {
        list_add(plugin_state->tokens_list, token_info);
    }
    plugin_state->tokens_count++;
    totp_config_file_save_new_token(token_info);

    if (load_generate_token_scene) {
        totp_scene_director_activate_scene(plugin_state, TotpSceneGenerateToken, NULL);
    }

    furi_string_free(temp_str);

    printf("Token \"%s\" has been successfully added\r\n", token_info->name);
}