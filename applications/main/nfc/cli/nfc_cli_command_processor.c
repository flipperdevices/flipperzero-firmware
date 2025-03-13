#include "nfc_cli_command_processor.h"
#include "nfc_cli_commands.h"
//#include "nfc_cli_command_base_i.h"
#include "nfc_cli.h"

#define TAG "NfcCliProcessor"

typedef struct {
    const NfcCliCommandDescriptor* cmd;
    const NfcCliActionDescriptor* action;
} NfcCliProcessorContext;

typedef bool (*NfcCliCommandProcessCallback)(FuriString* args, NfcCliProcessorContext* context);

static bool nfc_cli_get_action(FuriString* argument, NfcCliProcessorContext* context) {
    context->action = nfc_cli_command_get_action_by_name(context->cmd, argument);

    bool result = true;
    if(context->action == NULL) {
        FURI_LOG_E(TAG, "Unknown action %s", furi_string_get_cstr(argument));
        result = false;
    }

    return result;
}

static bool nfc_cli_parse_argument(FuriString* argument, NfcCliProcessorContext* context) {
    UNUSED(argument);
    UNUSED(context);
    return true;
}

/* void nfc_cli_command_process(PipeSide* pipe, FuriString* args, NfcCliProcessorContext* context)  */
void nfc_cli_command_process(const NfcCliCommandDescriptor* cmd, FuriString* args, void* context) {
    furi_assert(cmd);
    furi_assert(args);
    UNUSED(context);

    NfcCliProcessorContext* instance = malloc(sizeof(NfcCliProcessorContext));
    //context->cmd = nfc_cli_command_get_by_index(context->cmd_index);

    NfcCliCommandProcessCallback process =
        nfc_cli_command_has_multiple_actions(cmd) ? nfc_cli_get_action : nfc_cli_parse_argument;

    FuriString* arg_str = furi_string_alloc();
    while(args_read_string_and_trim(args, arg_str)) {
        bool result = process(arg_str, instance);
        if(!result) {
            ///TODO: maybe print error message
            ///and usage then
            break;
        }
        process = nfc_cli_parse_argument;
        // printf("%s\r\n", furi_string_get_cstr(arg_str));
    }
    furi_string_free(arg_str);
    free(instance);
}
