#include <furi.h>
#include <furi_hal.h>
#include <cli/cli.h>
#include <lib/toolbox/args.h>
#include <lib/toolbox/hex.h>

#include <furi_hal_nfc.h>

#include <nfc/nfc.h>
#include <nfc/nfc_scanner.h>
#include <nfc/nfc_poller.h>

#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#include <m-array.h>

#define FLAG_EVENT (1 << 10)

typedef struct {
    size_t protocols_detected_num;
    NfcProtocol protocols_detected[NfcProtocolNum];
} NfcScannerContext;

typedef struct {
    Nfc* nfc;
    FuriThreadId thread_id;
    NfcScannerContext scanner_context;
} NfcCliContext;

NfcCliContext* nfc_cli_context_alloc() {
    NfcCliContext* instance = malloc(sizeof(NfcCliContext));
    instance->nfc = nfc_alloc();
    instance->thread_id = furi_thread_get_current_id();

    return instance;
}

static const char* protocol_names[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = "iso14443-3a",
    [NfcProtocolIso14443_3b] = "iso14443-3b",
    [NfcProtocolIso14443_4a] = "iso14443-4a",
    [NfcProtocolIso14443_4b] = "iso14443-4b",
    [NfcProtocolIso15693_3] = "iso15693-3",
    [NfcProtocolFelica] = "felica",
    [NfcProtocolMfUltralight] = "mfu",
    [NfcProtocolMfClassic] = "mfc",
    [NfcProtocolMfDesfire] = "mfdes",
    [NfcProtocolSlix] = "slix",
    [NfcProtocolSt25tb] = "st25tb",
};

void nfc_cli_context_free(NfcCliContext* instance) {
    nfc_free(instance->nfc);
    free(instance);
}

static void nfc_cli_print_usage() {
    printf("Usage:\r\n");
    printf("nfc <cmd>\r\n");
    printf("Cmd list:\r\n");
    printf("\tdetect\t - detect NFC tag\r\n");
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        printf("\tfield\t - turn field on\r\n");
    }
}

void nfc_cli_detect_scanner_callback(NfcScannerEvent event, void* context) {
    NfcCliContext* instance = context;

    if(event.type == NfcScannerEventTypeDetected) {
        instance->scanner_context.protocols_detected_num = event.data.protocol_num;
        memcpy(
            instance->scanner_context.protocols_detected,
            event.data.protocols,
            event.data.protocol_num * sizeof(NfcProtocol));
        furi_thread_flags_set(instance->thread_id, FLAG_EVENT);
    }
}

static void nfc_cli_detect(Cli* cli, FuriString* args) {
    UNUSED(args);
    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_hal_ready() != FuriHalNfcErrorNone) {
        printf("NFC chip failed to start\r\n");
        return;
    }

    NfcCliContext* instance = nfc_cli_context_alloc();
    NfcScanner* scanner = nfc_scanner_alloc(instance->nfc);

    nfc_scanner_start(scanner, nfc_cli_detect_scanner_callback, instance);

    printf("Press Ctrl+C to abort\r\n\n");
    while(!cli_cmd_interrupt_received(cli)) {
        uint32_t event = furi_thread_flags_wait(FLAG_EVENT, FuriFlagWaitAny, 50);
        if(event == FLAG_EVENT) break;
    }
    nfc_scanner_stop(scanner);

    size_t protocols_detected = instance->scanner_context.protocols_detected_num;
    if(protocols_detected) {
        printf("Detected %d protocols:\r\n", protocols_detected);
        for(size_t i = 0; i < protocols_detected; i++) {
            printf("%s\r\n", protocol_names[instance->scanner_context.protocols_detected[i]]);
        }
    }

    nfc_scanner_free(scanner);
    nfc_cli_context_free(instance);
}

typedef struct {
    bool activation_required;
    bool crc_required;
    uint32_t timeout;
} PollCommandSettings;

typedef struct {
    BitBuffer* buffer_tx;
    uint32_t timeout_fc;
    bool append_crc;
} PollCommand;

ARRAY_DEF(PollCommandArray, PollCommand, M_POD_OPLIST);

typedef struct {
    Nfc* nfc;
    NfcPoller* poller;
    FuriMessageQueue* cli_queue;
    FuriMessageQueue* poller_queue;
    BitBuffer* tx_data;
    BitBuffer* rx_data;
} NfcCliPollContext;

typedef enum {
    NfcCliUserMessageTypeAbort,
    NfcCliUserMessageTypeActivate,
    NfcCliUserMessageTypeTx,
    NfcCliUserMessageTypeReset,
} NfcCliUserMessageType;

typedef struct {
    BitBuffer* tx_data;
    bool add_crc;
    uint32_t timeout_fc;
} NfcCliUserMessageData;

typedef struct {
    NfcCliUserMessageType type;
    NfcCliUserMessageData data;
} NfcCliUserMessage;

typedef enum {
    NfcCliWorkerMessageTypeError,
    NfcCliWorkerMessageTypeRx,
} NfcCliWorkerMessageType;

typedef enum {
    NfcCliWorkerErrorNone,
    NfcCliWorkerErrorTimeout,
    NfcCliWorkerErrorInternal,
} NfcCliWorkerError;

typedef struct {
    NfcCliWorkerError error;
} NfcCliWorkerMessageDataError;

typedef struct {
    BitBuffer* rx_data;
} NfcCLiWorkerMessageDataRx;

typedef union {
    NfcCliWorkerMessageDataError error;
    NfcCLiWorkerMessageDataRx rx_data;
} NfcCLiWorkerMessageData;

typedef struct {
    NfcCliWorkerMessageType type;
    NfcCLiWorkerMessageData data;
} NfcCliWorkerMessage;

NfcCliPollContext* nfc_cli_poller_context_alloc(NfcProtocol protocol) {
    NfcCliPollContext* instance = malloc(sizeof(NfcCliPollContext));
    instance->nfc = nfc_alloc();
    instance->poller = nfc_poller_alloc(instance->nfc, protocol);
    instance->cli_queue = furi_message_queue_alloc(8, sizeof(NfcCliWorkerMessage));
    instance->poller_queue = furi_message_queue_alloc(8, sizeof(NfcCliUserMessage));
    instance->tx_data = bit_buffer_alloc(255);
    instance->rx_data = bit_buffer_alloc(255);

    return instance;
}

void nfc_cli_poller_context_free(NfcCliPollContext* instance) {
    furi_assert(instance);

    nfc_poller_stop(instance->poller);

    bit_buffer_free(instance->tx_data);
    bit_buffer_free(instance->rx_data);
    furi_message_queue_free(instance->poller_queue);
    furi_message_queue_free(instance->cli_queue);
    nfc_poller_free(instance->poller);
    nfc_free(instance->nfc);
}

static void nfc_cli_iso14443_3a_print_usage() {
    printf("Usage:\r\n");
    printf("iso14443-3a poll <hex_data>:\r\n");
}

static NfcCliWorkerError nfc_cli_iso14443_3a_process_error(Iso14443_3aError error) {
    NfcCliWorkerError ret = NfcCliWorkerErrorNone;

    switch(error) {
    case Iso14443_3aErrorNone:
        ret = NfcCliWorkerErrorNone;
        break;
    case Iso14443_3aErrorTimeout:
    case Iso14443_3aErrorNotPresent:
        ret = NfcCliWorkerErrorTimeout;
        break;
    default:
        ret = NfcCliWorkerErrorInternal;
        break;
    }

    return ret;
}

static NfcCommand nfc_cli_iso14443_3a_poller_callback(NfcGenericEventEx event, void* context) {
    furi_assert(event.poller);
    furi_assert(context);

    NfcCommand command = NfcCommandContinue;

    NfcCliPollContext* instance = context;
    NfcEvent* nfc_event = event.parent_event_data;
    NfcCliUserMessage rx_message = {};
    NfcCliWorkerMessage tx_message = {};
    if(nfc_event->type == NfcEventTypePollerReady) {
        // TODO handle return
        furi_message_queue_get(instance->poller_queue, &rx_message, FuriWaitForever);
        if(rx_message.type == NfcCliUserMessageTypeAbort) {
            command = NfcCommandStop;

            tx_message.type = NfcCliWorkerMessageTypeError;
            tx_message.data.error.error = NfcCliWorkerErrorNone;
            // TODO handle return
            furi_message_queue_put(instance->cli_queue, &tx_message, FuriWaitForever);
        } else if(rx_message.type == NfcCliUserMessageTypeReset) {
            command = NfcCommandReset;
            tx_message.type = NfcCliWorkerMessageTypeError;
            tx_message.data.error.error = NfcCliWorkerErrorNone;
            // TODO handle return
            furi_message_queue_put(instance->cli_queue, &tx_message, FuriWaitForever);
        } else if(rx_message.type == NfcCliUserMessageTypeActivate) {
            Iso14443_3aData iso3_data = {};
            Iso14443_3aError error = iso14443_3a_poller_activate(event.poller, &iso3_data);

            tx_message.type = NfcCliWorkerMessageTypeError;
            tx_message.data.error.error = nfc_cli_iso14443_3a_process_error(error);
            // TODO handle return
            furi_message_queue_put(instance->cli_queue, &tx_message, FuriWaitForever);
        } else if(rx_message.type == NfcCliUserMessageTypeTx) {
            Iso14443_3aError error = Iso14443_3aErrorNone;
            if(rx_message.data.add_crc) {
                error = iso14443_3a_poller_send_standard_frame(
                    event.poller,
                    rx_message.data.tx_data,
                    instance->rx_data,
                    rx_message.data.timeout_fc);
                if(error == Iso14443_3aErrorNone) {
                    tx_message.type = NfcCliWorkerMessageTypeRx;
                    tx_message.data.rx_data.rx_data = instance->rx_data;
                    // TODO handle return
                    furi_message_queue_put(instance->cli_queue, &tx_message, FuriWaitForever);
                } else {
                    tx_message.type = NfcCliWorkerMessageTypeError;
                    tx_message.data.error.error = nfc_cli_iso14443_3a_process_error(error);
                    // TODO handle return
                    furi_message_queue_put(instance->cli_queue, &tx_message, FuriWaitForever);
                }
            } else {
                error = iso14443_3a_poller_txrx(
                    event.poller,
                    rx_message.data.tx_data,
                    instance->rx_data,
                    rx_message.data.timeout_fc);
                if(error == Iso14443_3aErrorNone) {
                    tx_message.type = NfcCliWorkerMessageTypeRx;
                    tx_message.data.rx_data.rx_data = instance->rx_data;
                    // TODO handle return
                    furi_message_queue_put(instance->cli_queue, &tx_message, FuriWaitForever);
                } else {
                    tx_message.type = NfcCliWorkerMessageTypeError;
                    tx_message.data.error.error = nfc_cli_iso14443_3a_process_error(error);
                    // TODO handle return
                    furi_message_queue_put(instance->cli_queue, &tx_message, FuriWaitForever);
                }
            }
        }
    }

    return command;
}

static void nfc_cli_iso14443_3a_poll_handler(FuriString* args) {
    FuriString* tmp_str = furi_string_alloc();

    size_t cmd_ascii_len = 0;
    PollCommandSettings settings = {
        .activation_required = true,
        .crc_required = true,
        .timeout = 200000,
    };
    PollCommandArray_t cmd_arr;
    PollCommandArray_init(cmd_arr);
    PollCommandArray_it_t iter;

    do {
        bool read_data_success = true;
        while(true) {
            cmd_ascii_len = args_get_first_word_length(args);
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
            BitBuffer* buff = bit_buffer_alloc(256);
            bit_buffer_copy_bytes(buff, data, cmd_ascii_len / 2);
            PollCommand cmd = {
                .append_crc = settings.crc_required,
                .timeout_fc = settings.timeout,
                .buffer_tx = buff,
            };
            PollCommandArray_push_back(cmd_arr, cmd);
        }

        if(!read_data_success) {
            nfc_cli_iso14443_3a_print_usage();
            break;
        }

        if(PollCommandArray_size(cmd_arr) == 0) {
            printf("No data to send\r\n");
            nfc_cli_iso14443_3a_print_usage();
            break;
        }

        printf(
            "\r\nParsed command: Activate: %d, Add CRC: %d",
            settings.activation_required,
            settings.crc_required);
        for(PollCommandArray_it(iter, cmd_arr); !PollCommandArray_end_p(iter);
            PollCommandArray_next(iter)) {
            const PollCommand* cmd = PollCommandArray_cref(iter);
            size_t bytes = bit_buffer_get_size_bytes(cmd->buffer_tx);
            printf("\r\nPoll %d bytes:\r\n", bit_buffer_get_size_bytes(cmd->buffer_tx));
            for(size_t i = 0; i < bytes; i++) {
                printf("%02X ", bit_buffer_get_byte(cmd->buffer_tx, i));
            }
            printf("\r\n\r\n");
        }

        NfcCliPollContext* instance = nfc_cli_poller_context_alloc(NfcProtocolIso14443_3a);
        nfc_poller_start_ex(instance->poller, nfc_cli_iso14443_3a_poller_callback, instance);

        NfcCliUserMessage tx_message = {};
        NfcCliWorkerMessage rx_message = {};

        if(settings.activation_required) {
            tx_message.type = NfcCliUserMessageTypeActivate;
            // TODO check return
            furi_message_queue_put(instance->poller_queue, &tx_message, FuriWaitForever);
            furi_message_queue_get(instance->cli_queue, &rx_message, FuriWaitForever);
            if(!(rx_message.type == NfcCliWorkerMessageTypeError &&
                 rx_message.data.error.error == NfcCliWorkerErrorNone)) {
                printf("Activation failed with error %d\r\n", rx_message.data.error.error);

                tx_message.type = NfcCliUserMessageTypeAbort;
                // TODO check return
                furi_message_queue_put(instance->poller_queue, &tx_message, FuriWaitForever);
                furi_message_queue_get(instance->cli_queue, &rx_message, FuriWaitForever);
                nfc_cli_poller_context_free(instance);
                break;
            }
        }

        for(PollCommandArray_it(iter, cmd_arr); !PollCommandArray_end_p(iter);
            PollCommandArray_next(iter)) {
            const PollCommand* cmd = PollCommandArray_cref(iter);
            printf("\r\nTx:");
            for(size_t i = 0; i < bit_buffer_get_size_bytes(cmd->buffer_tx); i++) {
                printf(" %02X", bit_buffer_get_byte(cmd->buffer_tx, i));
            }

            tx_message.type = NfcCliUserMessageTypeTx;
            tx_message.data.tx_data = cmd->buffer_tx;
            tx_message.data.add_crc = cmd->append_crc;
            tx_message.data.timeout_fc = cmd->timeout_fc;
            // TODO check return
            furi_message_queue_put(instance->poller_queue, &tx_message, FuriWaitForever);
            furi_message_queue_get(instance->cli_queue, &rx_message, FuriWaitForever);

            if(rx_message.type == NfcCliWorkerMessageTypeError) {
                printf("\r\nRx: error %d", rx_message.data.error.error);
                break;
            } else if(rx_message.type == NfcCliWorkerMessageTypeRx) {
                printf("\r\nRx:");
                for(size_t i = 0; i < bit_buffer_get_size_bytes(rx_message.data.rx_data.rx_data);
                    i++) {
                    printf(" %02X", bit_buffer_get_byte(rx_message.data.rx_data.rx_data, i));
                }
            }
        }

        tx_message.type = NfcCliUserMessageTypeAbort;
        // TODO check return
        furi_message_queue_put(instance->poller_queue, &tx_message, FuriWaitForever);
        furi_message_queue_get(instance->cli_queue, &rx_message, FuriWaitForever);

        nfc_cli_poller_context_free(instance);
    } while(false);

    for(PollCommandArray_it(iter, cmd_arr); !PollCommandArray_end_p(iter);
        PollCommandArray_next(iter)) {
        const PollCommand* cmd = PollCommandArray_cref(iter);
        bit_buffer_free(cmd->buffer_tx);
    }
    PollCommandArray_clear(cmd_arr);
    furi_string_free(tmp_str);
}

static void nfc_cli_iso14443_3a_start_poller_usage() {
    printf("Available commands:\r\n");
    printf("activate - activates tag\r\n");
    printf("poll - transieve and receive frames\r\n");
    printf("reset - resets the field\r\n");
    printf("help - displays command list\r\n");
    printf("Press Ctrl+C to abort\r\n");
}

static bool nfc_cli_start_poller_process_input(Cli* cli, FuriString* cmd) {
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
        }
    }

    return exit;
}

typedef void (*NfcCliPollerStartHandler)(FuriString* cmd, NfcCliPollContext* context);

typedef struct {
    const char* cmd;
    NfcCliPollerStartHandler handler;
} NfcPollerStartCmdHandler;

static void nfc_cli_iso14443_3a_dump_activation_data(
    const NfcDeviceData* data,
    FuriString* activation_info) {
    furi_assert(data);
    furi_assert(activation_info);

    const Iso14443_3aData* iso3_data = data;
    furi_string_printf(activation_info, "UID:");
    for(size_t i = 0; i < iso3_data->uid_len; i++) {
        furi_string_cat_printf(activation_info, " %02X", iso3_data->uid[i]);
    }
    furi_string_cat_printf(
        activation_info,
        " ATQA: %02X%02X SAK: %02X",
        iso3_data->atqa[0],
        iso3_data->atqa[1],
        iso3_data->sak);
}

void nfc_cli_poller_start_handler_activate(FuriString* cmd, NfcCliPollContext* context) {
    UNUSED(cmd);
    NfcCliPollContext* instance = context;
    NfcCliWorkerMessage rx_message = {};
    NfcCliUserMessage tx_message = {
        .type = NfcCliUserMessageTypeActivate,
    };
    // TODO process return
    furi_message_queue_put(instance->poller_queue, &tx_message, FuriWaitForever);
    furi_message_queue_get(instance->cli_queue, &rx_message, FuriWaitForever);
    if(rx_message.type == NfcCliWorkerMessageTypeError &&
       rx_message.data.error.error == NfcCliWorkerErrorNone) {
        const NfcDeviceData* nfc_data = nfc_poller_get_data(instance->poller);
        nfc_cli_iso14443_3a_dump_activation_data(nfc_data, cmd);
        printf("\r\nActivation succes: %s\r\n", furi_string_get_cstr(cmd));
    } else {
        printf("\r\nActivation failed with error %d\r\n", rx_message.data.error.error);
    }
    fflush(stdout);
}

void nfc_cli_poller_start_handler_poll(FuriString* cmd, NfcCliPollContext* context) {
    UNUSED(cmd);
    UNUSED(context);
}

void nfc_cli_poller_start_handler_reset(FuriString* cmd, NfcCliPollContext* context) {
    UNUSED(cmd);
    UNUSED(context);
}

NfcPollerStartCmdHandler nfc_cli_poller_start_handlers[] = {
    {
        .cmd = "activate",
        .handler = nfc_cli_poller_start_handler_activate,
    },
    {
        .cmd = "poll",
        .handler = nfc_cli_poller_start_handler_poll,
    },
    {
        .cmd = "reset",
        .handler = nfc_cli_poller_start_handler_reset,
    },
};

static void nfc_cli_iso14443_3a_start_poller_handler(Cli* cli) {
    FuriString* tmp_str = furi_string_alloc();
    FuriString* cmd_str = furi_string_alloc();

    printf("Poller started\r\n");
    nfc_cli_iso14443_3a_start_poller_usage();
    NfcCliPollContext* instance = nfc_cli_poller_context_alloc(NfcProtocolIso14443_3a);
    nfc_poller_start_ex(instance->poller, nfc_cli_iso14443_3a_poller_callback, instance);

    while(!nfc_cli_start_poller_process_input(cli, tmp_str)) {
        if(!args_read_string_and_trim(tmp_str, cmd_str)) {
            continue;
        }

        bool command_found = false;
        for(size_t i = 0; i < COUNT_OF(nfc_cli_poller_start_handlers); i++) {
            if(furi_string_cmp_str(cmd_str, nfc_cli_poller_start_handlers[i].cmd) == 0) {
                nfc_cli_poller_start_handlers[i].handler(tmp_str, instance);
                command_found = true;
            }
        }
        if(!command_found) {
            nfc_cli_iso14443_3a_start_poller_usage();
        }
    }

    NfcCliUserMessage rx_message = {};
    NfcCliUserMessage tx_message = {
        .type = NfcCliUserMessageTypeAbort,
    };
    // TODO process return
    furi_message_queue_put(instance->poller_queue, &tx_message, FuriWaitForever);
    furi_message_queue_get(instance->cli_queue, &rx_message, FuriWaitForever);

    printf("Poller stopped\r\n");

    nfc_cli_poller_context_free(instance);
    furi_string_free(cmd_str);
    furi_string_free(tmp_str);
}

static void nfc_cli_iso14443_3a(Cli* cli, FuriString* args) {
    FuriString* tmp_str = furi_string_alloc();

    do {
        if(!args_read_string_and_trim(args, tmp_str)) {
            nfc_cli_iso14443_3a_print_usage();
            break;
        }
        if(furi_string_cmp_str(tmp_str, "poll") == 0) {
            nfc_cli_iso14443_3a_poll_handler(args);
        } else if(furi_string_cmp_str(tmp_str, "start_poller") == 0) {
            nfc_cli_iso14443_3a_start_poller_handler(cli);
        } else {
            nfc_cli_iso14443_3a_print_usage();
            break;
        }

    } while(false);

    furi_string_free(tmp_str);
}

static void nfc_cli_field(Cli* cli, FuriString* args) {
    UNUSED(args);
    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_hal_ready() != FuriHalNfcErrorNone) {
        printf("NFC chip failed to start\r\n");
        return;
    }

    furi_hal_nfc_acquire();
    furi_hal_nfc_low_power_mode_stop();
    furi_hal_nfc_poller_field_on();

    printf("Field is on. Don't leave device in this mode for too long.\r\n");
    printf("Press Ctrl+C to abort\r\n");

    while(!cli_cmd_interrupt_received(cli)) {
        furi_delay_ms(50);
    }

    furi_hal_nfc_low_power_mode_start();
    furi_hal_nfc_release();
}

static void nfc_cli(Cli* cli, FuriString* args, void* context) {
    UNUSED(context);
    FuriString* cmd;
    cmd = furi_string_alloc();

    do {
        if(!args_read_string_and_trim(args, cmd)) {
            nfc_cli_print_usage();
            break;
        }
        if(furi_string_cmp_str(cmd, "detect") == 0) {
            nfc_cli_detect(cli, args);
            break;
        }
        if(furi_string_cmp_str(cmd, "iso14443-3a") == 0) {
            nfc_cli_iso14443_3a(cli, args);
            break;
        }
        if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
            if(furi_string_cmp_str(cmd, "field") == 0) {
                nfc_cli_field(cli, args);
                break;
            }
        }

        nfc_cli_print_usage();
    } while(false);

    furi_string_free(cmd);
}

void nfc_on_system_start() {
#ifdef SRV_CLI
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_add_command(cli, "nfc", CliCommandFlagDefault, nfc_cli, NULL);
    furi_record_close(RECORD_CLI);
#else
    UNUSED(nfc_cli);
#endif
}
