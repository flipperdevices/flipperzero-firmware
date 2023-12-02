#include <flipper_application/flipper_application.h>
#include <lib/toolbox/args.h>
#include "../../cli_helpers.h"
#include "../../cli_shared_methods.h"
#include "../../cli_plugin_interface.h"
#include "../../../services/config/config.h"
#include "../../../services/kb_layouts/kb_layout_provider.h"
#include "../../../ui/scene_director.h"
#include "../../../config/app/config.h"

#define TOTP_CLI_COMMAND_AUTOMATION_ARG_METHOD "automation"
#define TOTP_CLI_COMMAND_AUTOMATION_METHOD_NONE "none"
#define TOTP_CLI_COMMAND_AUTOMATION_METHOD_USB "usb"
#ifdef TOTP_BADBT_AUTOMATION_ENABLED
#define TOTP_CLI_COMMAND_AUTOMATION_METHOD_BT "bt"
#endif
#define TOTP_CLI_COMMAND_AUTOMATION_ARG_KB_LAYOUT_PREFIX "-k"
#define TOTP_CLI_COMMAND_AUTOMATION_ARG_INITIAL_DELAY_PREFIX "-w"

static void print_method(AutomationMethod method, const char* color) {
#ifdef TOTP_BADBT_AUTOMATION_ENABLED
    bool has_previous_method = false;
#endif
    if(method & AutomationMethodBadUsb) {
        TOTP_CLI_PRINTF_COLORFUL(color, "\"" TOTP_CLI_COMMAND_AUTOMATION_METHOD_USB "\"");
#ifdef TOTP_BADBT_AUTOMATION_ENABLED
        has_previous_method = true;
#endif
    }

#ifdef TOTP_BADBT_AUTOMATION_ENABLED
    if(method & AutomationMethodBadBt) {
        if(has_previous_method) {
            TOTP_CLI_PRINTF_COLORFUL(color, " and ");
        }

        TOTP_CLI_PRINTF_COLORFUL(color, "\"" TOTP_CLI_COMMAND_AUTOMATION_METHOD_BT "\"");
    }
#endif

    if(method == AutomationMethodNone) {
        TOTP_CLI_PRINTF_COLORFUL(color, "\"" TOTP_CLI_COMMAND_AUTOMATION_METHOD_NONE "\"");
    }
}

static void print_kb_layout(AutomationKeyboardLayout layout, const char* color) {
    char layoutToPrint[TOTP_KB_LAYOUT_NAME_MAX_LENGTH + 1];
    totp_kb_layout_provider_get_layout_name(layout, &layoutToPrint[0], sizeof(layoutToPrint));
    TOTP_CLI_PRINTF_COLORFUL(color, "%s", &layoutToPrint[0]);
}

static void print_initial_delay(uint16_t initial_delay, const char* color) {
    double delay_sec = initial_delay / 1000.0;
    TOTP_CLI_PRINTF_COLORFUL(color, "%.1f", delay_sec);
}

static void handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    if(!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    FuriString* temp_str = furi_string_alloc();
    bool new_method_provided = false;
    AutomationMethod new_method = AutomationMethodNone;
    AutomationKeyboardLayout new_kb_layout = plugin_state->automation_kb_layout;
    uint16_t new_initial_delay = plugin_state->automation_initial_delay;
    bool args_valid = true;
    while(args_read_string_and_trim(args, temp_str)) {
        if(furi_string_cmpi_str(temp_str, TOTP_CLI_COMMAND_AUTOMATION_METHOD_NONE) == 0) {
            new_method_provided = true;
            new_method = AutomationMethodNone;
        } else if(furi_string_cmpi_str(temp_str, TOTP_CLI_COMMAND_AUTOMATION_METHOD_USB) == 0) {
            new_method_provided = true;
            new_method |= AutomationMethodBadUsb;
        }
#ifdef TOTP_BADBT_AUTOMATION_ENABLED
        else if(furi_string_cmpi_str(temp_str, TOTP_CLI_COMMAND_AUTOMATION_METHOD_BT) == 0) {
            new_method_provided = true;
            new_method |= AutomationMethodBadBt;
        }
#endif
        else if(furi_string_cmpi_str(temp_str, TOTP_CLI_COMMAND_AUTOMATION_ARG_KB_LAYOUT_PREFIX) == 0) {
            if(!args_read_string_and_trim(args, temp_str) ||
               !totp_kb_layout_provider_get_layout_by_name(
                   furi_string_get_cstr(temp_str), &new_kb_layout)) {
                args_valid = false;
                break;
            }
        } else if(
            furi_string_cmpi_str(temp_str, TOTP_CLI_COMMAND_AUTOMATION_ARG_INITIAL_DELAY_PREFIX) ==
            0) {
            float temp_float;
            char* strtof_endptr;
            if(args_read_string_and_trim(args, temp_str) &&
               (temp_float = strtof(furi_string_get_cstr(temp_str), &strtof_endptr)) >= 0 &&
               *strtof_endptr == 0) {
                new_initial_delay = (uint16_t)(temp_float * 1000.0f);
            } else {
                args_valid = false;
                break;
            }
        } else {
            args_valid = false;
            break;
        }
    }

    do {
        if(!args_valid) {
            TOTP_CLI_PRINT_INVALID_ARGUMENTS();
            break;
        }

        if(new_method_provided || new_kb_layout != plugin_state->automation_kb_layout ||
           new_initial_delay != plugin_state->automation_initial_delay) {
            TOTP_CLI_LOCK_UI(plugin_state);

            if(new_method_provided) {
                plugin_state->automation_method = new_method;
            }

            plugin_state->automation_kb_layout = new_kb_layout;
            plugin_state->automation_initial_delay = new_initial_delay;
            if(totp_config_file_update_automation_method(plugin_state)) {
                TOTP_CLI_PRINTF_SUCCESS("Automation method is set to ");
                print_method(plugin_state->automation_method, TOTP_CLI_COLOR_SUCCESS);
                TOTP_CLI_PRINTF_SUCCESS(" (");
                print_kb_layout(plugin_state->automation_kb_layout, TOTP_CLI_COLOR_SUCCESS);
                TOTP_CLI_PRINTF_SUCCESS(")");
                TOTP_CLI_PRINTF_SUCCESS(" [");
                print_initial_delay(
                    plugin_state->automation_initial_delay, TOTP_CLI_COLOR_SUCCESS);
                TOTP_CLI_PRINTF_SUCCESS(" sec.]");
                cli_nl();
            } else {
                TOTP_CLI_PRINT_ERROR_UPDATING_CONFIG_FILE();
            }

#ifdef TOTP_BADBT_AUTOMATION_ENABLED
            if(!(new_method & AutomationMethodBadBt) &&
               plugin_state->bt_type_code_worker_context != NULL) {
                totp_bt_type_code_worker_free(plugin_state->bt_type_code_worker_context);
                plugin_state->bt_type_code_worker_context = NULL;
            }
#endif

            TOTP_CLI_UNLOCK_UI(plugin_state);
        } else {
            TOTP_CLI_PRINTF_INFO("Current automation method is ");
            print_method(plugin_state->automation_method, TOTP_CLI_COLOR_INFO);
            TOTP_CLI_PRINTF_INFO(" (");
            print_kb_layout(plugin_state->automation_kb_layout, TOTP_CLI_COLOR_INFO);
            TOTP_CLI_PRINTF_INFO(")");
            TOTP_CLI_PRINTF_INFO(" [");
            print_initial_delay(plugin_state->automation_initial_delay, TOTP_CLI_COLOR_INFO);
            TOTP_CLI_PRINTF_INFO(" sec.]");
            cli_nl();
        }
    } while(false);

    furi_string_free(temp_str);
}

static const CliPlugin plugin = {.name = "TOTP CLI Plugin: Automation", .handle = &handle};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin,
};

const FlipperAppPluginDescriptor* totp_cli_automation_plugin_ep() {
    return &plugin_descriptor;
}
