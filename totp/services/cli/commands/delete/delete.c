#include "delete.h"

#include <stdlib.h>
#include <ctype.h>
#include <lib/toolbox/args.h>
#include "../../../list/list.h"
#include "../../../config/config.h"
#include "../../cli_common_helpers.h"
#include "../../../../scenes/scene_director.h"

void totp_cli_handle_delete_command(PluginState* plugin_state, FuriString* args, Cli* cli) {
    int token_number;
    if (!args_read_int_and_trim(args, &token_number) || token_number <= 0 || token_number > plugin_state->tokens_count) {
        totp_cli_print_invalid_arguments();
        return;
    }

    FuriString* temp_str = furi_string_alloc();
    bool confirm_needed = true;
    if (args_read_string_and_trim(args, temp_str)) {
        if (furi_string_cmpi_str(temp_str, "-f") == 0) {
            confirm_needed = false;
        } else {
            printf("Unknown argument \"%s\"\r\n", furi_string_get_cstr(temp_str));
            totp_cli_print_invalid_arguments();
            furi_string_free(temp_str);
            return;
        }
    }
    furi_string_free(temp_str);

    ListNode* list_node = list_element_at(plugin_state->tokens_list, token_number - 1);

    TokenInfo* token_info = list_node->data;

    bool confirmed = !confirm_needed;
    if (confirm_needed) {
        printf("WARNING!\r\n");
        printf("Token \"%s\" will be permanently deleted without ability to recover it.\r\n", token_info->name);
        printf("Confirm? [y/n]\r\n");
        fflush(stdout);
        char user_pick;
        do {
            user_pick = tolower(cli_getc(cli));
        } while (user_pick != 'y' && user_pick != 'n' && user_pick != 0x0d);

        confirmed = user_pick == 'y' || user_pick == 0x0d;
    }

    if (confirmed) {
        bool activate_generate_token_scene = false;
        if (plugin_state->current_scene == TotpSceneGenerateToken) {
            totp_scene_director_activate_scene(plugin_state, TotpSceneNone, NULL);
            activate_generate_token_scene = true;
        }
        
        plugin_state->tokens_list = list_remove(plugin_state->tokens_list, list_node);
        plugin_state->tokens_count--;

        totp_full_save_config_file(plugin_state);
        
        if (activate_generate_token_scene) {
            totp_scene_director_activate_scene(plugin_state, TotpSceneGenerateToken, NULL);
        }

        printf("Token \"%s\" has been successfully deleted\r\n", token_info->name);
        token_info_free(token_info);
    } else {
        printf("User not confirmed\r\n");
    }
}