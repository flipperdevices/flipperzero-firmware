#include <furi.h>
#include <furi_hal.h>
#include <cli/cli.h>
#include <lib/toolbox/args.h>
#include <lib/toolbox/hex.h>

#include <drivers/st25r3916.h>
#include <nfc/nfc.h>
#include <f_hal_nfc.h>
#include <f_hal_nfc_i.h>

#include <furi_hal_resources.h>
#include <signal_reader/parsers/iso15693/iso15693_parser.h>

#define FLAG_EVENT (1 << 10)

static void nfc_cli_print_usage() {
    printf("Usage:\r\n");
    printf("nfc <cmd>\r\n");
    printf("Cmd list:\r\n");
    printf("\tdetect\t - detect nfc device\r\n");
    printf("\temulate\t - emulate predefined nfca card\r\n");
    printf("\tapdu\t - Send APDU and print response \r\n");
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        printf("\tfield\t - turn field on\r\n");
    }
}

static void f_hal_nfc_iso15693_listener_transparent_mode_enter(FuriHalSpiBusHandle* handle) {
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSPARENT_MODE);

    furi_hal_spi_bus_handle_deinit(handle);
    f_hal_nfc_deinit_gpio_isr();
}

static void f_hal_nfc_iso15693_listener_transparent_mode_exit(FuriHalSpiBusHandle* handle) {
    // Configure gpio back to SPI and exit transparent mode
    f_hal_nfc_init_gpio_isr();
    furi_hal_spi_bus_handle_init(handle);

    st25r3916_direct_cmd(handle, ST25R3916_CMD_UNMASK_RECEIVE_DATA);
}

static void f_hal_nfc_iso15693_parser_callback(Iso15693ParserEvent event, void* context) {
    furi_assert(context);

    if(event == Iso15693ParserEventDataReceived) {
        FuriThreadId thread_id = context;
        furi_thread_flags_set(thread_id, FLAG_EVENT);
    }
}

// TODO remove this test command
static void nfc_cli_check(Cli* cli, FuriString* args) {
    UNUSED(args);
    UNUSED(cli);
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    uint8_t data[100] = {};
    size_t bits = 0;

    Nfc* nfc = nfc_alloc();
    f_hal_nfc_low_power_mode_stop();
    f_hal_nfc_set_mode(FHalNfcModeListener, FHalNfcTechIso15693);
    f_hal_nfc_iso15693_listener_transparent_mode_enter(handle);
    Iso15693Parser* instance = iso15693_parser_alloc(&gpio_spi_r_miso, 1024);

    FuriThreadId thread_id = furi_thread_get_current_id();
    furi_thread_set_current_priority(FuriThreadPriorityHighest);
    iso15693_parser_start(instance, f_hal_nfc_iso15693_parser_callback, thread_id);

    while(true) {
        uint32_t flag = furi_thread_flags_wait(FLAG_EVENT, FuriFlagWaitAny, FuriWaitForever);
        furi_thread_flags_clear(flag);

        if(flag & FLAG_EVENT) {
            if(iso15693_parser_run(instance)) {
                iso15693_parser_get_data(instance, data, sizeof(data), &bits);
                break;
            }
        }
    }

    for(size_t i = 0; i < bits / 8; i++) {
        printf("%02X ", data[i]);
    }
    printf("\r\n");

    iso15693_parser_stop(instance);
    f_hal_nfc_iso15693_listener_transparent_mode_exit(handle);

    iso15693_parser_free(instance);
    f_hal_nfc_reset_mode();
    f_hal_nfc_low_power_mode_start();
    nfc_free(nfc);
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
        if(furi_string_cmp_str(cmd, "c") == 0) {
            nfc_cli_check(cli, args);
            break;
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
