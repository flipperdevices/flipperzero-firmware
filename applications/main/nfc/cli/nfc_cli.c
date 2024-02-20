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

#include "protocol_support/nfc_cli_protocol_support.h"

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
    printf("\tdetect\t\t - detect NFC tag\r\n");
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        printf("\tfield\t\t - turn field on\r\n");
    }
    nfc_cli_protocol_support_print_usage();
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
        if(nfc_cli_protocol_support_cmd_process(cli, args)) break;
        if(!args_read_string_and_trim(args, cmd)) {
            nfc_cli_print_usage();
            break;
        }
        if(furi_string_cmp_str(cmd, "detect") == 0) {
            nfc_cli_detect(cli, args);
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
