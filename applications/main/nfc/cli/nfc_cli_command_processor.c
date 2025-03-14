#include "nfc_cli_command_processor.h"
#include "nfc_cli_commands.h"
#include "nfc_cli_command_base_i.h"
#include "nfc_cli.h"

#define TAG "NfcCliProcessor"

struct NfcCliProcessorContext {
    const NfcCliCommandDescriptor* cmd;
    const NfcCliActionDescriptor* action;
    Nfc* nfc;
    void* action_context;
};

typedef bool (*NfcCliCommandProcessCallback)(FuriString* args, NfcCliProcessorContext* context);

/* static bool nfc_cli_get_action(FuriString* argument, NfcCliProcessorContext* context) {
    context->action = nfc_cli_command_get_action_by_name(context->cmd, argument);

    bool result = true;
    if(context->action == NULL) {
        FURI_LOG_E(TAG, "Unknown action %s", furi_string_get_cstr(argument));
        result = false;
    }

    return result;
} */

static const NfcCliActionDescriptor*
    nfc_cli_get_action_from_args(const NfcCliCommandDescriptor* cmd, FuriString* args) {
    const NfcCliActionDescriptor* action = cmd->actions[0];

    bool multiple_action_cmd = nfc_cli_command_has_multiple_actions(cmd);
    if(multiple_action_cmd) {
        action = NULL;
        FuriString* arg_str = furi_string_alloc();
        if(args_read_string_and_trim(args, arg_str)) {
            action = nfc_cli_command_get_action_by_name(cmd, arg_str);
        }
        furi_string_free(arg_str);
    }

    return action;
}

static bool nfc_cli_action_alloc(NfcCliProcessorContext* instance, FuriString* args) {
    bool result = false;
    const NfcCliCommandDescriptor* cmd = instance->cmd;
    const NfcCliActionDescriptor* action = nfc_cli_get_action_from_args(cmd, args);

    if(action) {
        instance->action = action;
        if(action->alloc) {
            FURI_LOG_D(TAG, "Allocation context for action \"%s\"", action->name);
            instance->action_context = instance->action->alloc(instance->nfc);
        } else {
            FURI_LOG_D(TAG, "No alloc context callback for action \"%s\"", action->name);
            instance->action_context = NULL;
        }
        result = true;
    }
    return result;
}

static bool nfc_cli_parse_argument(NfcCliProcessorContext* instance, FuriString* argument) {
    UNUSED(argument);
    UNUSED(instance);
    return true;
}

/* void nfc_cli_command_process(PipeSide* pipe, FuriString* args, NfcCliProcessorContext* context)  */
void nfc_cli_command_process(
    const NfcCliCommandDescriptor* cmd,
    PipeSide* pipe,
    FuriString* args,
    void* context) {
    furi_assert(pipe);
    furi_assert(cmd);
    furi_assert(args);
    NfcCliProcessorContext* instance = context;

    instance->cmd = cmd;
    do {
        if(!nfc_cli_action_alloc(instance, args)) break;

        //context->cmd = nfc_cli_command_get_by_index(context->cmd_index);

        FuriString* argument = furi_string_alloc();
        while(args_read_string_and_trim(args, argument)) {
            bool result = nfc_cli_parse_argument(instance, argument);
            if(!result) {
                ///TODO: maybe print error message
                ///and usage then
                break;
            }

            // printf("%s\r\n", furi_string_get_cstr(arg_str));
        }
        furi_string_free(argument);
    } while(false);
    ///TODO: move to commands file as a function with check not NULL and logs

    if(instance->action && instance->action->execute) {
        instance->action->execute(pipe, instance->action_context);
    } else {
        FURI_LOG_D(TAG, "Action or execute callback missing");
    }
}

NfcCliProcessorContext* nfc_cli_command_processor_alloc(Nfc* nfc) {
    furi_assert(nfc);
    NfcCliProcessorContext* instance = malloc(sizeof(NfcCliProcessorContext));
    instance->nfc = nfc;
    return instance;
}

void nfc_cli_command_processor_free(NfcCliProcessorContext* instance) {
    furi_assert(instance);
    instance->nfc = NULL;
    free(instance);
}
