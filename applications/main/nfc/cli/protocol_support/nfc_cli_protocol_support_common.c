#include "nfc_cli_protocol_support_common_i.h"

#include <furi/furi.h>
#include <toolbox/args.h>

static void nfc_cli_protocol_support_poll_print_usage() {
    printf("Usage:\r\n");
    printf("iso14443-3a poll <hex_data>:\r\n");
}

static NfcCliPollerContext* nfc_cli_poller_context_alloc(NfcProtocol protocol) {
    NfcCliPollerContext* instance = malloc(sizeof(NfcCliPollerContext));
    instance->nfc = nfc_alloc();
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
    nfc_free(instance->nfc);
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

void nfc_cli_protocol_support_common_poll_handler(
    NfcProtocol protocol,
    Cli* cli,
    FuriString* args,
    NfcCliProtocolSupportCommonCallback callback) {
    UNUSED(cli);
    furi_assert(protocol < NfcProtocolNum);
    furi_assert(args);
    furi_assert(callback);

    FuriString* tmp_str = furi_string_alloc();
    NfcCliPollCmdDataArray_t cmd_arr;
    NfcCliPollCmdDataArray_init(cmd_arr);
    NfcCliPollCmdDataArray_it_t iter;
    NfcCliPollerContext* instance = nfc_cli_poller_context_alloc(protocol);

    do {
        // TODO parse parameters
        NfcCliPollCmdParameters params = {
            .activation_required = true,
            .append_crc = true,
            .timeout = 200000,
        };

        bool read_data_success = true;
        while(true) {
            size_t cmd_ascii_len = args_get_first_word_length(args);
            if((cmd_ascii_len % 2) != 0) {
                printf("Incorrect data length: %d\r\n", cmd_ascii_len);
                read_data_success = false;
                break;
            }
            if(cmd_ascii_len == 0) break;

            if(!args_read_string_and_trim(args, tmp_str)) {
                printf("Failed to read data\r\n");
                read_data_success = false;
                break;
            }

            if(!args_read_hex_bytes(tmp_str, instance->buffer, cmd_ascii_len / 2)) {
                printf("Failed to read hex bytes\r\n");
                read_data_success = false;
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

        if(!read_data_success) {
            nfc_cli_protocol_support_poll_print_usage();
            break;
        }

        if(NfcCliPollCmdDataArray_size(cmd_arr) == 0) {
            printf("No data to send\r\n");
            nfc_cli_protocol_support_poll_print_usage();
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

static bool nfc_cli_protocol_support_common_process_input(Cli* cli, FuriString* cmd) {
    bool exit = false;
    furi_string_reset(cmd);

    printf("... ");
    fflush(stdout);

    char c = 0;
    while(cli_read(cli, (uint8_t*)&c, 1) == 1) {
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

void nfc_cli_protocol_support_common_start_poller_handle_poll(
    FuriString* cmd,
    NfcCliPollerContext* instance) {
    FuriString* tmp_str = furi_string_alloc();

    NfcCliPollCmdData poll_cmd_data = {
        .tx_data = bit_buffer_alloc(NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE),
        .append_crc = true,
        .timeout = 200000,
    };

    do {
        // TODO parse params here
        size_t cmd_ascii_len = args_get_first_word_length(cmd);
        if((cmd_ascii_len % 2) != 0) {
            printf("Incorrect data length: %d\r\n", cmd_ascii_len);
            break;
        }
        if(cmd_ascii_len == 0) break;

        if(!args_read_string_and_trim(cmd, tmp_str)) {
            printf("Failed to read data\r\n");
            break;
        }

        if(!args_read_hex_bytes(tmp_str, instance->buffer, cmd_ascii_len / 2)) {
            printf("Failed to read hex bytes\r\n");
            break;
        }

        bit_buffer_copy_bytes(poll_cmd_data.tx_data, instance->buffer, cmd_ascii_len / 2);

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
    Cli* cli,
    FuriString* args,
    NfcCliProtocolSupportCommonCallback callback) {
    furi_assert(protocol < NfcProtocolNum);
    furi_assert(cli);
    furi_assert(args);
    furi_assert(callback);

    printf("Poller started\r\n");
    nfc_cli_protocol_support_common_start_poller_print_usage();

    FuriString* tmp_str = furi_string_alloc();
    FuriString* cmd_str = furi_string_alloc();
    NfcCliPollerContext* instance = nfc_cli_poller_context_alloc(protocol);
    instance->callback = callback;
    nfc_poller_start_ex(instance->poller, nfc_cli_protocol_support_poll_worker_callback, instance);

    while(!nfc_cli_protocol_support_common_process_input(cli, tmp_str)) {
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
