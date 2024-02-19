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

    return instance;
}

static void nfc_cli_poller_context_free(NfcCliPollerContext* instance) {
    furi_assert(instance);

    nfc_poller_stop(instance->poller);

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
            };
            instance->callback(&activate_request);

            NfcCliUserMessage tx_message = {
                .type = NfcCliUserMessageTypeStatus,
                .data.error = activate_request.data.error,
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

            uint8_t data[256] = {};
            if(!args_read_hex_bytes(tmp_str, data, cmd_ascii_len / 2)) {
                printf("Failed to read hex bytes\r\n");
                read_data_success = false;
                break;
            }

            BitBuffer* buff = bit_buffer_alloc(cmd_ascii_len / 2);
            bit_buffer_copy_bytes(buff, data, cmd_ascii_len / 2);
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

        NfcCliPollerContext* instance = nfc_cli_poller_context_alloc(protocol);
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
                nfc_cli_poller_context_free(instance);
                break;
            }

            NfcCliProtocolRequest request = {
                .type = NfcCliProtocolRequestTypeActivateInfo,
                .data.activation_info =
                    {
                        .dev_data = nfc_poller_get_data(instance->poller),
                        .formatted_data = tmp_str,
                    },
            };
            instance->callback(&request);
            printf(
                "Activation success: %s\r\n",
                furi_string_get_cstr(request.data.activation_info.formatted_data));
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

        nfc_cli_poller_context_free(instance);
    } while(false);

    for(NfcCliPollCmdDataArray_it(iter, cmd_arr); !NfcCliPollCmdDataArray_end_p(iter);
        NfcCliPollCmdDataArray_next(iter)) {
        const NfcCliPollCmdData* cmd = NfcCliPollCmdDataArray_cref(iter);
        bit_buffer_free(cmd->tx_data);
    }
    NfcCliPollCmdDataArray_clear(cmd_arr);
    furi_string_free(tmp_str);
}

// void nfc_cli_protocol_support_common_start_poller_handler(
//     NfcProtocol protocol,
//     Cli* cli,
//     FuriString* args,
//     NfcCliProtocolSupportCommonCallback callback) {
//     furi_assert(protocol < NfcProtocolNum);
//     furi_assert(cli);
//     furi_assert(args);
//     furi_assert(callback);

// }
