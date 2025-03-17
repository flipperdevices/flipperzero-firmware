#include "nfc_cli_command_processor.h"
#include "nfc_cli_commands.h"
#include "nfc_cli_command_base_i.h"
#include "nfc_cli.h"

#define TAG "NfcCliProcessor"

#define NFC_CLI_KEYS_FOUND_SIZE_BYTES (10 * sizeof(NfcCliKeyDescriptor*))

typedef enum {
    NfcCliArgumentTypeShortNameKey,
    NfcCliArgumentTypeShortNameKeyGroup,
    NfcCliArgumentTypeLongNameKey,

    NfcCliArgumentTypeUnknown
} NfcCliArgumentType;

struct NfcCliProcessorContext {
    const NfcCliCommandDescriptor* cmd;
    const NfcCliActionDescriptor* action;
    const NfcCliKeyDescriptor** keys_found;

    uint8_t total_keys_found;
    uint8_t required_keys_expected;
    uint8_t required_keys_found;

    Nfc* nfc;
    void* action_context;
};

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
    const NfcCliCommandDescriptor* cmd = instance->cmd;

    bool result = false;
    do {
        const NfcCliActionDescriptor* action = nfc_cli_get_action_from_args(cmd, args);
        if(action == NULL) break;

        instance->action = action;
        memset(instance->keys_found, 0, NFC_CLI_KEYS_FOUND_SIZE_BYTES);
        instance->required_keys_expected = nfc_cli_action_get_required_keys_count(action);
        instance->required_keys_found = 0;
        instance->total_keys_found = 0;

        if(action->alloc) {
            FURI_LOG_D(TAG, "Allocation context for action \"%s\"", action->name);
            instance->action_context = instance->action->alloc(instance->nfc);
        } else {
            FURI_LOG_D(TAG, "No alloc context callback for action \"%s\"", action->name);
            instance->action_context = NULL;
        }
        result = true;
    } while(false);

    return result;
}

static NfcCliArgumentType nfc_cli_get_argument_type(FuriString* argument) {
    size_t arg_len = furi_string_size(argument);
    NfcCliArgumentType type = NfcCliArgumentTypeUnknown;

    if(arg_len > 2) {
        char ch1 = furi_string_get_char(argument, 0);
        char ch2 = furi_string_get_char(argument, 1);
        if(ch1 == '-') {
            type = (ch2 == '-') ? NfcCliArgumentTypeLongNameKey :
                                  NfcCliArgumentTypeShortNameKeyGroup;

        } else {
            type = NfcCliArgumentTypeUnknown;
        }
    } else if(arg_len == 2) {
        char ch1 = furi_string_get_char(argument, 0);
        type = (ch1 == '-') ? NfcCliArgumentTypeShortNameKey : NfcCliArgumentTypeUnknown;
    }

    return type;
}

static bool
    nfc_cli_check_duplicate_keys(NfcCliProcessorContext* instance, const NfcCliKeyDescriptor* key) {
    bool result = false;
    for(size_t i = 0; i < instance->total_keys_found; i++) {
        const NfcCliKeyDescriptor* buf = instance->keys_found[i];
        if(buf != key) continue;
        result = true;
        break;
    }

    return result;
}

static bool nfc_cli_parse_single_key(
    NfcCliProcessorContext* instance,
    FuriString* argument,
    FuriString* args,
    bool from_group) {
    bool result = false;
    FuriString* value_str = furi_string_alloc();

    do {
        const NfcCliKeyDescriptor* key =
            nfc_cli_action_get_key_descriptor(instance->action, argument);
        if(key == NULL) break;

        if(key->features.parameter && from_group) break;

        if(nfc_cli_check_duplicate_keys(instance, key)) break;

        if(key->features.parameter && !args_read_string_and_trim(args, value_str)) break;

        FURI_LOG_D(TAG, "Parsing key \"%s\"", furi_string_get_cstr(argument));
        if(!key->parse(value_str, instance->action_context)) break;

        instance->keys_found[instance->total_keys_found] = key;
        instance->total_keys_found++;
        if(key->features.required) instance->required_keys_found++;

        result = true;
    } while(false);
    furi_string_free(value_str);

    return result;
}

static bool nfc_cli_parse_group_key(NfcCliProcessorContext* instance, FuriString* argument) {
    bool result = false;
    FURI_LOG_D(TAG, "Parsing key group\"%s\"", furi_string_get_cstr(argument));

    FuriString* arg_buf = furi_string_alloc();
    for(size_t i = 0; i < furi_string_size(argument); i++) {
        furi_string_set_n(arg_buf, argument, i, 1);
        result = nfc_cli_parse_single_key(instance, arg_buf, NULL, true);
        if(!result) break;
    }
    furi_string_free(arg_buf);

    return result;
}

static bool nfc_cli_parse_argument(
    NfcCliProcessorContext* instance,
    FuriString* argument,
    FuriString* args) {
    NfcCliArgumentType type = nfc_cli_get_argument_type(argument);

    furi_string_trim(argument, "-");

    bool result = false;

    if(type == NfcCliArgumentTypeShortNameKeyGroup)
        result = nfc_cli_parse_group_key(instance, argument);
    else if((type == NfcCliArgumentTypeShortNameKey) || (type == NfcCliArgumentTypeLongNameKey)) {
        result = nfc_cli_parse_single_key(instance, argument, args, false);
    }

    return result;
}

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

        FuriString* argument = furi_string_alloc();
        while(args_read_string_and_trim(args, argument)) {
            ///TODO: this result should be moved outside and used as one of the criteria can we execute command or not.
            bool result = nfc_cli_parse_argument(instance, argument, args);
            if(!result) {
                ///TODO: maybe print error message
                ///and usage then
                break;
            }

            // printf("%s\r\n", furi_string_get_cstr(arg_str));
        }
        furi_string_free(argument);

        if(instance->required_keys_expected != instance->required_keys_found) {
            FURI_LOG_W(TAG, "Some required keys missing");
            //print usage
            break;
        }

        if(instance->action && instance->action->execute) {
            instance->action->execute(pipe, instance->action_context);
        } else {
            FURI_LOG_D(TAG, "Action or execute callback missing");
        }
    } while(false);
}

NfcCliProcessorContext* nfc_cli_command_processor_alloc(Nfc* nfc) {
    furi_assert(nfc);
    NfcCliProcessorContext* instance = malloc(sizeof(NfcCliProcessorContext));
    instance->nfc = nfc;
    ///TODO: think of using MLIB instead of handling on your own
    instance->keys_found = malloc(NFC_CLI_KEYS_FOUND_SIZE_BYTES);
    instance->total_keys_found = 0;
    instance->required_keys_found = 0;
    instance->required_keys_expected = 0;

    return instance;
}

void nfc_cli_command_processor_free(NfcCliProcessorContext* instance) {
    furi_assert(instance);
    instance->nfc = NULL;
    free(instance->keys_found);
    free(instance);
}
