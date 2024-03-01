#include "nfc_cli_protocol_support_common_i.h"

#include <furi/furi.h>
#include <toolbox/args.h>
#include <toolbox/arg_parser.h>

static NfcCliPollerContext* nfc_cli_poller_context_alloc(Nfc* nfc, NfcProtocol protocol) {
    NfcCliPollerContext* instance = malloc(sizeof(NfcCliPollerContext));
    instance->nfc = nfc;
    instance->poller = nfc_poller_alloc(instance->nfc, protocol);
    instance->user_queue = furi_message_queue_alloc(8, sizeof(NfcCliUserMessage));
    instance->worker_queue = furi_message_queue_alloc(8, sizeof(NfcCliWorkerMessage));
    instance->rx_data = bit_buffer_alloc(NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE);
    instance->formatted_data = furi_string_alloc();

    return instance;
}

static void nfc_cli_poller_context_free(NfcCliPollerContext* instance) {
    furi_assert(instance);

    furi_string_free(instance->formatted_data);
    bit_buffer_free(instance->rx_data);
    furi_message_queue_free(instance->worker_queue);
    furi_message_queue_free(instance->user_queue);
    nfc_poller_free(instance->poller);
}

static NfcCommand
    nfc_cli_protocol_support_poll_worker_callback(NfcGenericEventEx event, void* context) {
    NfcCommand command = NfcCommandContinue;

    NfcCliPollerContext* instance = context;
    NfcEvent* nfc_event = event.parent_event_data;
    NfcCliWorkerMessage rx_message = {};

    if(nfc_event->type == NfcEventTypePollerReady) {
        furi_check(
            furi_message_queue_get(instance->worker_queue, &rx_message, FuriWaitForever) ==
            FuriStatusOk);
        if(rx_message.type == NfcCliWorkerMessageTypeAbort) {
            command = NfcCommandStop;

            NfcCliUserMessage tx_message = {
                .type = NfcCliUserMessageTypeStatus,
                .data.error = NfcCliPollerErrorNone,
            };
            furi_check(
                furi_message_queue_put(instance->user_queue, &tx_message, FuriWaitForever) ==
                FuriStatusOk);
        } else if(rx_message.type == NfcCliWorkerMessageTypeReset) {
            command = NfcCommandReset;

            NfcCliUserMessage tx_message = {
                .type = NfcCliUserMessageTypeStatus,
                .data.error = NfcCliPollerErrorNone,
            };
            furi_check(
                furi_message_queue_put(instance->user_queue, &tx_message, FuriWaitForever) ==
                FuriStatusOk);
        } else if(rx_message.type == NfcCliWorkerMessageTypeActivate) {
            NfcCliProtocolRequest activate_request = {
                .type = NfcCliProtocolRequestTypeActivate,
                .data.nfc = instance->nfc,
                .data.poller = event.poller,
                .data.activation_info = instance->formatted_data,
            };
            instance->callback(&activate_request);

            NfcCliUserMessage tx_message = {
                .type = NfcCliUserMessageTypeStatus,
                .data.error = activate_request.data.error,
                .data.formatted_data = instance->formatted_data,
            };
            furi_check(
                furi_message_queue_put(instance->user_queue, &tx_message, FuriWaitForever) ==
                FuriStatusOk);
        } else if(rx_message.type == NfcCliWorkerMessageTypeFrameExchange) {
            NfcCliProtocolRequest frame_exchange_request = {
                .type = NfcCliProtocolRequestTypeFrameExchange,
                .data =
                    {
                        .nfc = instance->nfc,
                        .poller = event.poller,
                        .frame_exchange =
                            {
                                .tx_data = rx_message.data.tx_data,
                                .rx_data = instance->rx_data,

                            },
                    },
            };
            instance->callback(&frame_exchange_request);

            NfcCliUserMessage tx_message = {
                .type = NfcCliUserMessageTypeRx,
            };
            if(frame_exchange_request.data.error == NfcCliPollerErrorNone) {
                tx_message.data.error = NfcCliPollerErrorNone;
                tx_message.data.rx_data = instance->rx_data;
            } else {
                tx_message.data.error = frame_exchange_request.data.error;
            }
            furi_check(
                furi_message_queue_put(instance->user_queue, &tx_message, FuriWaitForever) ==
                FuriStatusOk);
        }
    }

    return command;
}

static const ArgParserOptions nfc_cli_protocol_support_common_poll_options[] = {
    {
        .identifier = 'a',
        .access_letters = "a",
        .access_name = "activate",
        .value_name = NULL,
        .description = "Pass activation procedure",
    },
    {
        .identifier = 'c',
        .access_letters = "c",
        .access_name = NULL,
        .value_name = NULL,
        .description = "Append CRC",
    },
    {
        .identifier = 't',
        .access_letters = "t",
        .access_name = "timeout",
        .value_name = "TIMEOUT",
        .description = "Timeout in fc",
    },
    {
        .identifier = 'h',
        .access_letters = "h",
        .access_name = "help",
        .description = "Shows the command help",
    },
};

void nfc_cli_protocol_support_common_poll_handler(
    NfcProtocol protocol,
    NfcCli* nfc_cli,
    FuriString* args,
    NfcCliProtocolSupportCommonCallback callback) {
    UNUSED(nfc_cli);
    furi_assert(protocol < NfcProtocolNum);
    furi_assert(args);
    furi_assert(callback);

    FuriString* tmp_str = furi_string_alloc();
    NfcCliPollCmdDataArray_t cmd_arr;
    NfcCliPollCmdDataArray_init(cmd_arr);
    NfcCliPollCmdDataArray_it_t iter;
    NfcCliPollerContext* instance = nfc_cli_poller_context_alloc(nfc_cli->nfc, protocol);

    NfcCliPollCmdParameters params = {
        .activation_required = false,
        .append_crc = false,
        .timeout = 200000,
    };

    ArgParser* parser = arg_parser_alloc(
        nfc_cli_protocol_support_common_poll_options,
        COUNT_OF(nfc_cli_protocol_support_common_poll_options),
        args);

    bool read_success = true;
    do {
        while(arg_parser_fetch(parser) && read_success) {
            switch(arg_parser_get_identifier(parser)) {
            case 'a':
                params.activation_required = true;
                break;
            case 'c':
                params.append_crc = true;
                break;
            case 't':
                if(sscanf(arg_parser_get_value(parser), "%lu", &params.timeout) != 1) {
                    read_success = false;
                }
                break;
            case 'h':
                printf("Usage: poll\r\n");
                arg_parser_get_help_message(parser, tmp_str);
                printf("%s\r\n", furi_string_get_cstr(tmp_str));
                break;
            case '?':
                arg_parser_get_error_message(parser, tmp_str);
                printf("%s\r\n", furi_string_get_cstr(tmp_str));
                read_success = false;
                break;
            }
        }
        if(!read_success) break;

        while(true) {
            const char* additional_args = arg_parser_get_next_argument(parser);
            if(additional_args == NULL) break;

            size_t cmd_ascii_len = strlen(additional_args);
            if(((cmd_ascii_len % 2) != 0) || (cmd_ascii_len == 0)) {
                printf(
                    "Incorrect data length %zu in argument: %s\r\n",
                    cmd_ascii_len,
                    additional_args);
                read_success = false;
                break;
            }

            furi_string_set_str(tmp_str, additional_args);
            if(!args_read_hex_bytes(tmp_str, instance->buffer, cmd_ascii_len / 2)) {
                printf("Failed to read hex bytes\r\n");
                read_success = false;
                break;
            }

            BitBuffer* buff = bit_buffer_alloc(cmd_ascii_len / 2);
            bit_buffer_copy_bytes(buff, instance->buffer, cmd_ascii_len / 2);
            NfcCliPollCmdData cmd = {
                .append_crc = params.append_crc,
                .timeout = params.timeout,
                .tx_data = buff,
            };
            NfcCliPollCmdDataArray_push_back(cmd_arr, cmd);
        }

        if(!read_success) {
            printf("Usage: nfc poll\r\n");
            arg_parser_get_help_message(parser, tmp_str);
            printf("%s\r\n", furi_string_get_cstr(tmp_str));
            break;
        }

        if(NfcCliPollCmdDataArray_size(cmd_arr) == 0) {
            printf("No data to send\r\n");
            arg_parser_get_help_message(parser, tmp_str);
            printf("%s\r\n", furi_string_get_cstr(tmp_str));
            break;
        }

        instance->callback = callback;
        nfc_poller_start_ex(
            instance->poller, nfc_cli_protocol_support_poll_worker_callback, instance);

        NfcCliUserMessage rx_message = {};
        if(params.activation_required) {
            NfcCliWorkerMessage tx_message = {
                .type = NfcCliWorkerMessageTypeActivate,
            };
            furi_check(
                furi_message_queue_put(instance->worker_queue, &tx_message, FuriWaitForever) ==
                FuriStatusOk);
            furi_check(
                furi_message_queue_get(instance->user_queue, &rx_message, FuriWaitForever) ==
                FuriStatusOk);

            if(!(rx_message.type == NfcCliUserMessageTypeStatus &&
                 rx_message.data.error == NfcCliPollerErrorNone)) {
                printf("Activation failed with error %d\r\n", rx_message.data.error);

                tx_message.type = NfcCliWorkerMessageTypeAbort;
                furi_check(
                    furi_message_queue_put(instance->worker_queue, &tx_message, FuriWaitForever) ==
                    FuriStatusOk);
                furi_check(
                    furi_message_queue_get(instance->user_queue, &rx_message, FuriWaitForever) ==
                    FuriStatusOk);
                nfc_poller_stop(instance->poller);
                break;
            }

            printf(
                "Activation success: %s\r\n",
                furi_string_get_cstr(rx_message.data.formatted_data));
        }

        for(NfcCliPollCmdDataArray_it(iter, cmd_arr); !NfcCliPollCmdDataArray_end_p(iter);
            NfcCliPollCmdDataArray_next(iter)) {
            const NfcCliPollCmdData* cmd = NfcCliPollCmdDataArray_cref(iter);
            printf("\r\nTx:");
            for(size_t i = 0; i < bit_buffer_get_size_bytes(cmd->tx_data); i++) {
                printf(" %02X", bit_buffer_get_byte(cmd->tx_data, i));
            }

            NfcCliWorkerMessage tx_message = {
                .type = NfcCliWorkerMessageTypeFrameExchange,
                .data.tx_data = cmd,
            };
            furi_check(
                furi_message_queue_put(instance->worker_queue, &tx_message, FuriWaitForever) ==
                FuriStatusOk);
            furi_check(
                furi_message_queue_get(instance->user_queue, &rx_message, FuriWaitForever) ==
                FuriStatusOk);

            if(!((rx_message.type == NfcCliUserMessageTypeRx) &&
                 (rx_message.data.error == NfcCliPollerErrorNone))) {
                printf("\r\nRx: error %d", rx_message.data.error);
                break;
            } else {
                printf("\r\nRx:");
                for(size_t i = 0; i < bit_buffer_get_size_bytes(rx_message.data.rx_data); i++) {
                    printf(" %02X", bit_buffer_get_byte(rx_message.data.rx_data, i));
                }
            }
        }

        NfcCliWorkerMessage tx_message = {
            .type = NfcCliWorkerMessageTypeAbort,
        };
        furi_check(
            furi_message_queue_put(instance->worker_queue, &tx_message, FuriWaitForever) ==
            FuriStatusOk);
        furi_check(
            furi_message_queue_get(instance->user_queue, &rx_message, FuriWaitForever) ==
            FuriStatusOk);

        nfc_poller_stop(instance->poller);
    } while(false);

    arg_parser_free(parser);
    nfc_cli_poller_context_free(instance);
    for(NfcCliPollCmdDataArray_it(iter, cmd_arr); !NfcCliPollCmdDataArray_end_p(iter);
        NfcCliPollCmdDataArray_next(iter)) {
        const NfcCliPollCmdData* cmd = NfcCliPollCmdDataArray_cref(iter);
        bit_buffer_free(cmd->tx_data);
    }
    NfcCliPollCmdDataArray_clear(cmd_arr);
    furi_string_free(tmp_str);
}

static void nfc_cli_protocol_support_common_start_poller_print_usage() {
    printf("Available commands:\r\n");
    printf("activate - activates tag\r\n");
    printf("poll - transieve and receive frames\r\n");
    printf("reset - resets the field\r\n");
    printf("help - displays command list\r\n");
    printf("Press Ctrl+C to abort\r\n");
}

static bool nfc_cli_protocol_support_common_process_input(NfcCli* nfc_cli, FuriString* cmd) {
    bool exit = false;
    furi_string_reset(cmd);

    printf("... ");
    fflush(stdout);

    char c = 0;
    while(nfc_cli_read(nfc_cli, (uint8_t*)&c, 1) == 1) {
        if(c == CliSymbolAsciiETX) {
            printf("\r\n");
            exit = true;
            break;
        } else if(c >= 0x20 && c < 0x7F) {
            putc(c, stdout);
            fflush(stdout);
            furi_string_push_back(cmd, c);
        } else if(c == CliSymbolAsciiCR) {
            furi_string_trim(cmd);
            if(furi_string_size(cmd)) {
                printf("\r\n");
                fflush(stdout);
                break;
            } else {
                printf("\r\n... ");
                fflush(stdout);
            }
        } else if(c == CliSymbolAsciiBackspace) {
            size_t len = furi_string_size(cmd);
            if(len > 0) {
                // Other side
                printf("\e[D\e[1P");
                fflush(stdout);
                // Our side
                furi_string_left(cmd, len - 1);
            }
        }
    }

    return exit;
}

void nfc_cli_protocol_support_common_start_poller_handle_activate(
    FuriString* cmd,
    NfcCliPollerContext* instance) {
    UNUSED(cmd);
    NfcCliUserMessage rx_message = {};
    NfcCliWorkerMessage tx_message = {
        .type = NfcCliWorkerMessageTypeActivate,
    };

    furi_check(
        furi_message_queue_put(instance->worker_queue, &tx_message, FuriWaitForever) ==
        FuriStatusOk);
    furi_check(
        furi_message_queue_get(instance->user_queue, &rx_message, FuriWaitForever) ==
        FuriStatusOk);

    if(rx_message.type == NfcCliUserMessageTypeStatus &&
       rx_message.data.error == NfcCliPollerErrorNone) {
        printf(
            "\r\nActivation succes: %s\r\n", furi_string_get_cstr(rx_message.data.formatted_data));
    } else {
        printf("\r\nActivation failed with error %d\r\n", rx_message.data.error);
    }
    fflush(stdout);
}

static const ArgParserOptions nfc_cli_protocol_support_common_start_poller_poll_options[] = {
    {
        .identifier = 'c',
        .access_letters = "c",
        .access_name = NULL,
        .value_name = NULL,
        .description = "Append CRC",
    },
    {
        .identifier = 'b',
        .access_letters = "b",
        .access_name = "bits",
        .value_name = "BITS",
        .description = "Bits in last byte",
    },
    {
        .identifier = 't',
        .access_letters = "t",
        .access_name = "timeout",
        .value_name = "TIMEOUT",
        .description = "Timeout in fc",
    },
    {
        .identifier = 'h',
        .access_letters = "h",
        .access_name = "help",
        .description = "Shows the command help",
    },
};

void nfc_cli_protocol_support_common_start_poller_handle_poll(
    FuriString* cmd,
    NfcCliPollerContext* instance) {
    FuriString* tmp_str = furi_string_alloc();

    NfcCliPollCmdData poll_cmd_data = {
        .tx_data = bit_buffer_alloc(NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE),
        .append_crc = false,
        .timeout = 200000,
    };
    uint32_t bits_in_last_byte = 8;

    ArgParser* parser = arg_parser_alloc(
        nfc_cli_protocol_support_common_start_poller_poll_options,
        COUNT_OF(nfc_cli_protocol_support_common_start_poller_poll_options),
        cmd);

    bool read_success = true;
    do {
        while(arg_parser_fetch(parser) && read_success) {
            switch(arg_parser_get_identifier(parser)) {
            case 'b':
                if(sscanf(arg_parser_get_value(parser), "%lu", &bits_in_last_byte) != 1) {
                    read_success = false;
                    break;
                }
                if((bits_in_last_byte == 0) || (bits_in_last_byte > 8)) {
                    printf("Bits must be in range [1..7]");
                    read_success = false;
                    break;
                }
                break;
            case 'c':
                poll_cmd_data.append_crc = true;
                break;
            case 't':
                if(sscanf(arg_parser_get_value(parser), "%lu", &poll_cmd_data.timeout) != 1) {
                    read_success = false;
                }
                break;
            case 'h':
                printf("Usage: poll\r\n");
                arg_parser_get_help_message(parser, tmp_str);
                printf("%s\r\n", furi_string_get_cstr(tmp_str));
                break;
            case '?':
                arg_parser_get_error_message(parser, tmp_str);
                printf("%s\r\n", furi_string_get_cstr(tmp_str));
                read_success = false;
                break;
            }
        }
        if(!read_success) break;

        const char* additional_args = arg_parser_get_next_argument(parser);
        if(additional_args == NULL) {
            read_success = false;
            break;
        }
        size_t cmd_ascii_len = strlen(additional_args);

        if(((cmd_ascii_len % 2) != 0) || (cmd_ascii_len == 0)) {
            printf(
                "Incorrect data length %zu in argument: %s\r\n", cmd_ascii_len, additional_args);
            read_success = false;
            break;
        }

        furi_string_set_str(tmp_str, additional_args);
        if(!args_read_hex_bytes(tmp_str, instance->buffer, cmd_ascii_len / 2)) {
            printf("Failed to read hex bytes\r\n");
            break;
        }

        bit_buffer_copy_bytes(poll_cmd_data.tx_data, instance->buffer, cmd_ascii_len / 2);
        size_t total_bits = (cmd_ascii_len / 2) * 8 - (8 - bits_in_last_byte);
        bit_buffer_set_size(poll_cmd_data.tx_data, total_bits);

        printf("\r\nTx:");
        for(size_t i = 0; i < bit_buffer_get_size_bytes(poll_cmd_data.tx_data); i++) {
            printf(" %02X", bit_buffer_get_byte(poll_cmd_data.tx_data, i));
        }

        NfcCliUserMessage rx_message = {};
        NfcCliWorkerMessage tx_message = {
            .type = NfcCliWorkerMessageTypeFrameExchange,
            .data.tx_data = &poll_cmd_data,
        };

        furi_check(
            furi_message_queue_put(instance->worker_queue, &tx_message, FuriWaitForever) ==
            FuriStatusOk);
        furi_check(
            furi_message_queue_get(instance->user_queue, &rx_message, FuriWaitForever) ==
            FuriStatusOk);

        if(rx_message.data.error != NfcCliPollerErrorNone) {
            printf("\r\nRx: error %d", rx_message.data.error);
        } else {
            printf("\r\nRx:");
            for(size_t i = 0; i < bit_buffer_get_size_bytes(rx_message.data.rx_data); i++) {
                printf(" %02X", bit_buffer_get_byte(rx_message.data.rx_data, i));
            }
        }
        printf("\r\n");

    } while(false);

    bit_buffer_free(poll_cmd_data.tx_data);
    furi_string_free(tmp_str);
}

void nfc_cli_protocol_support_common_start_poller_handle_reset(
    FuriString* cmd,
    NfcCliPollerContext* instance) {
    UNUSED(cmd);

    NfcCliUserMessage rx_message = {};
    NfcCliWorkerMessage tx_message = {
        .type = NfcCliWorkerMessageTypeReset,
    };

    furi_check(
        furi_message_queue_put(instance->worker_queue, &tx_message, FuriWaitForever) ==
        FuriStatusOk);
    furi_check(
        furi_message_queue_get(instance->user_queue, &rx_message, FuriWaitForever) ==
        FuriStatusOk);

    if(!(rx_message.type == NfcCliUserMessageTypeStatus &&
         rx_message.data.error == NfcCliPollerErrorNone)) {
        printf("\r\nReset failed with error %d\r\n", rx_message.data.error);
    }
}

NfcCliStartPollerCommand nfc_cli_protocol_support_common_start_poller_handlers[] = {
    {
        .cmd = "activate",
        .handler = nfc_cli_protocol_support_common_start_poller_handle_activate,
    },
    {
        .cmd = "poll",
        .handler = nfc_cli_protocol_support_common_start_poller_handle_poll,
    },
    {
        .cmd = "reset",
        .handler = nfc_cli_protocol_support_common_start_poller_handle_reset,
    },
};

void nfc_cli_protocol_support_common_start_poller_handler(
    NfcProtocol protocol,
    NfcCli* nfc_cli,
    FuriString* args,
    NfcCliProtocolSupportCommonCallback callback) {
    furi_assert(protocol < NfcProtocolNum);
    furi_assert(nfc_cli);
    furi_assert(args);
    furi_assert(callback);

    printf("Poller started\r\n");
    nfc_cli_protocol_support_common_start_poller_print_usage();

    FuriString* tmp_str = furi_string_alloc();
    FuriString* cmd_str = furi_string_alloc();
    NfcCliPollerContext* instance = nfc_cli_poller_context_alloc(nfc_cli->nfc, protocol);
    instance->callback = callback;
    nfc_poller_start_ex(instance->poller, nfc_cli_protocol_support_poll_worker_callback, instance);

    while(!nfc_cli_protocol_support_common_process_input(nfc_cli, tmp_str)) {
        if(!args_read_string_and_trim(tmp_str, cmd_str)) {
            continue;
        }

        bool command_found = false;
        for(size_t i = 0; i < COUNT_OF(nfc_cli_protocol_support_common_start_poller_handlers);
            i++) {
            if(furi_string_cmp_str(
                   cmd_str, nfc_cli_protocol_support_common_start_poller_handlers[i].cmd) == 0) {
                nfc_cli_protocol_support_common_start_poller_handlers[i].handler(
                    tmp_str, instance);
                command_found = true;
            }
        }
        if(!command_found) {
            nfc_cli_protocol_support_common_start_poller_print_usage();
        }
    }

    NfcCliUserMessage rx_message = {};
    NfcCliWorkerMessage tx_message = {
        .type = NfcCliWorkerMessageTypeAbort,
    };
    furi_check(
        furi_message_queue_put(instance->worker_queue, &tx_message, FuriWaitForever) ==
        FuriStatusOk);
    furi_check(
        furi_message_queue_get(instance->user_queue, &rx_message, FuriWaitForever) ==
        FuriStatusOk);

    printf("Poller stopped\r\n");

    nfc_poller_stop(instance->poller);
    nfc_cli_poller_context_free(instance);
    furi_string_free(cmd_str);
    furi_string_free(tmp_str);
}
