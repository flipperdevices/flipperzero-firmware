#include "common.h"
#include <lib/toolbox/args.h>
#include "stdint.h"
#include "../../cli_helpers.h"
#include "../../cli_shared_methods.h"

bool totp_cli_try_read_algo(TokenInfo* token_info, FuriString* arg, FuriString* args, bool* parsed) {
    if(furi_string_cmpi_str(arg, TOTP_CLI_COMMAND_ARG_ALGO_PREFIX) == 0) {
        if(!args_read_string_and_trim(args, arg)) {
            totp_cli_printf_missed_argument_value(TOTP_CLI_COMMAND_ARG_ALGO_PREFIX);
        } else if(!token_info_set_algo_from_str(token_info, arg)) {
            TOTP_CLI_PRINTF_ERROR(
                "\"%s\" is incorrect value for argument \"" TOTP_CLI_COMMAND_ARG_ALGO_PREFIX
                "\"\r\n",
                furi_string_get_cstr(arg));
        } else {
            *parsed = true;
        }

        return true;
    }

    return false;
}

bool totp_cli_try_read_digits(
    TokenInfo* token_info,
    const FuriString* arg,
    FuriString* args,
    bool* parsed) {
    if(furi_string_cmpi_str(arg, TOTP_CLI_COMMAND_ARG_DIGITS_PREFIX) == 0) {
        uint8_t digit_value;
        if(!args_read_uint8_and_trim(args, &digit_value)) {
            totp_cli_printf_missed_argument_value(TOTP_CLI_COMMAND_ARG_DIGITS_PREFIX);
        } else if(!token_info_set_digits_from_int(token_info, digit_value)) {
            TOTP_CLI_PRINTF_ERROR(
                "\"%" PRIu8
                "\" is incorrect value for argument \"" TOTP_CLI_COMMAND_ARG_DIGITS_PREFIX
                "\"\r\n",
                digit_value);
        } else {
            *parsed = true;
        }

        return true;
    }

    return false;
}

bool totp_cli_try_read_duration(
    TokenInfo* token_info,
    const FuriString* arg,
    FuriString* args,
    bool* parsed) {
    if(furi_string_cmpi_str(arg, TOTP_CLI_COMMAND_ARG_DURATION_PREFIX) == 0) {
        uint8_t duration_value;
        if(!args_read_uint8_and_trim(args, &duration_value)) {
            totp_cli_printf_missed_argument_value(TOTP_CLI_COMMAND_ARG_DURATION_PREFIX);
        } else if(!token_info_set_duration_from_int(token_info, duration_value)) {
            TOTP_CLI_PRINTF_ERROR(
                "\"%" PRIu8
                "\" is incorrect value for argument \"" TOTP_CLI_COMMAND_ARG_DURATION_PREFIX
                "\"\r\n",
                duration_value);
        } else {
            *parsed = true;
        }

        return true;
    }

    return false;
}

bool totp_cli_try_read_automation_features(
    TokenInfo* token_info,
    FuriString* arg,
    FuriString* args,
    bool* parsed) {
    if(furi_string_cmpi_str(arg, TOTP_CLI_COMMAND_ARG_AUTOMATION_FEATURE_PREFIX) == 0) {
        if(!args_read_string_and_trim(args, arg)) {
            totp_cli_printf_missed_argument_value(TOTP_CLI_COMMAND_ARG_AUTOMATION_FEATURE_PREFIX);
        } else if(!token_info_set_automation_feature_from_str(token_info, arg)) {
            TOTP_CLI_PRINTF_ERROR(
                "\"%s\" is incorrect value for argument \"" TOTP_CLI_COMMAND_ARG_AUTOMATION_FEATURE_PREFIX
                "\"\r\n",
                furi_string_get_cstr(arg));
        } else {
            *parsed = true;
        }

        return true;
    }

    return false;
}

bool totp_cli_try_read_unsecure_flag(const FuriString* arg, bool* parsed, bool* unsecure_flag) {
    if(furi_string_cmpi_str(arg, TOTP_CLI_COMMAND_ARG_UNSECURE_PREFIX) == 0) {
        *unsecure_flag = false;
        *parsed = true;
        return true;
    }

    return false;
}

bool totp_cli_try_read_plain_token_secret_encoding(
    FuriString* arg,
    FuriString* args,
    bool* parsed,
    PlainTokenSecretEncoding* secret_encoding) {
    if(furi_string_cmpi_str(arg, TOTP_CLI_COMMAND_ARG_SECRET_ENCODING_PREFIX) == 0) {
        if(!args_read_string_and_trim(args, arg)) {
            totp_cli_printf_missed_argument_value(TOTP_CLI_COMMAND_ARG_SECRET_ENCODING_PREFIX);
        } else {
            if(furi_string_cmpi_str(arg, PLAIN_TOKEN_ENCODING_BASE32_NAME) == 0) {
                *secret_encoding = PlainTokenSecretEncodingBase32;
                *parsed = true;
            } else if(furi_string_cmpi_str(arg, PLAIN_TOKEN_ENCODING_BASE64_NAME) == 0) {
                *secret_encoding = PlainTokenSecretEncodingBase64;
                *parsed = true;
            } else {
                TOTP_CLI_PRINTF_ERROR(
                    "\"%s\" is incorrect value for argument \"" TOTP_CLI_COMMAND_ARG_SECRET_ENCODING_PREFIX
                    "\"\r\n",
                    furi_string_get_cstr(arg));
            }
        }

        return true;
    }

    return false;
}

bool totp_cli_try_read_token_type(
    TokenInfo* token_info,
    FuriString* arg,
    FuriString* args,
    bool* parsed) {
    if(furi_string_cmpi_str(arg, TOTP_CLI_COMMAND_ARG_TYPE_PREFIX) == 0) {
        if(!args_read_string_and_trim(args, arg)) {
            totp_cli_printf_missed_argument_value(TOTP_CLI_COMMAND_ARG_TYPE_PREFIX);
        } else if(!token_info_set_token_type_from_str(token_info, arg)) {
            TOTP_CLI_PRINTF_ERROR(
                "\"%s\" is incorrect value for argument \"" TOTP_CLI_COMMAND_ARG_TYPE_PREFIX
                "\"\r\n",
                furi_string_get_cstr(arg));
        } else {
            *parsed = true;
        }

        return true;
    }

    return false;
}

bool totp_cli_try_read_token_counter(
    TokenInfo* token_info,
    FuriString* arg,
    FuriString* args,
    bool* parsed) {
    if(furi_string_cmpi_str(arg, TOTP_CLI_COMMAND_ARG_COUNTER_PREFIX) == 0) {
        if(!args_read_string_and_trim(args, arg)) {
            totp_cli_printf_missed_argument_value(TOTP_CLI_COMMAND_ARG_COUNTER_PREFIX);
        } else if(!token_info_set_token_counter_from_str(token_info, arg)) {
            TOTP_CLI_PRINTF_ERROR(
                "\"%s\" is incorrect value for argument \"" TOTP_CLI_COMMAND_ARG_COUNTER_PREFIX
                "\"\r\n",
                furi_string_get_cstr(arg));
        } else {
            *parsed = true;
        }

        return true;
    }

    return false;
}
