#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_nfc.h>
#include <cli/cli.h>
#include <lib/toolbox/args.h>
#include <st25r3916.h>
#include <st25r3916_irq.h>
#include <furi_hal_spi.h>
#include <furi_hal_gpio.h>
#include <furi_hal_cortex.h>
#include <furi_hal_resources.h>

#include <lib/nfc/nfc_types.h>
#include <lib/nfc/nfc_device.h>



static void nfc_cli_print_usage() {
    printf("Usage:\r\n");
    printf("nfc <cmd>\r\n");
    printf("Cmd list:\r\n");
    printf("\tdetect\t - detect nfc device\r\n");
    printf("\temulate\t - emulate predefined nfca card\r\n");
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        printf("\tfield\t - turn field on\r\n");
        printf("\tst25r_read\t - read ST25R3916 register\r\n");
        printf("\tst25r_write\t - write ST25R3916 register\r\n");
        printf("\tst25r_cmd\t - execute ST25R3916 command\r\n");
        printf("\tst25r_fifo\t - wait for FIFO data\r\n");
    }
}

static void nfc_cli_detect(Cli* cli, FuriString* args) {
    UNUSED(args);
    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_busy()) {
        printf("Nfc is busy\r\n");
        return;
    }

    FuriHalNfcDevData dev_data = {};
    bool cmd_exit = false;
    furi_hal_nfc_exit_sleep();
    printf("Detecting nfc...\r\nPress Ctrl+C to abort\r\n");
    while(!cmd_exit) {
        cmd_exit |= cli_cmd_interrupt_received(cli);
        if(furi_hal_nfc_detect(&dev_data, 400)) {
            printf("found: %s ", nfc_get_dev_type(dev_data.type));
            printf("UID length: %d, UID:", dev_data.uid_len);
            for(size_t i = 0; i < dev_data.uid_len; i++) {
                printf("%02X", dev_data.uid[i]);
            }
            printf("\r\n");
            break;
        }
        furi_hal_nfc_sleep();
        furi_delay_ms(50);
    }
    furi_hal_nfc_sleep();
}

static void nfc_cli_emulate(Cli* cli, FuriString* args) {
    UNUSED(args);
    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_busy()) {
        printf("Nfc is busy\r\n");
        return;
    }

    furi_hal_nfc_exit_sleep();
    printf("Emulating NFC-A Type: T2T UID: 36 9C E7 B1 0A C1 34 SAK: 00 ATQA: 00/44\r\n");
    printf("Press Ctrl+C to abort\r\n");

    FuriHalNfcDevData params = {
        .uid = {0x36, 0x9C, 0xe7, 0xb1, 0x0A, 0xC1, 0x34},
        .uid_len = 7,
        .atqa = {0x44, 0x00},
        .sak = 0x00,
        .type = FuriHalNfcTypeA,
    };

    while(!cli_cmd_interrupt_received(cli)) {
        if(furi_hal_nfc_listen(params.uid, params.uid_len, params.atqa, params.sak, false, 100)) {
            printf("Reader detected\r\n");
            furi_hal_nfc_sleep();
        }
        furi_delay_ms(50);
    }
    furi_hal_nfc_sleep();
}

static void nfc_cli_field(Cli* cli, FuriString* args) {
    UNUSED(args);
    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_busy()) {
        printf("Nfc is busy\r\n");
        return;
    }

    furi_hal_nfc_exit_sleep();
    furi_hal_nfc_field_on();

    printf("Field is on. Don't leave device in this mode for too long.\r\n");
    printf("Press Ctrl+C to abort\r\n");

    while(!cli_cmd_interrupt_received(cli)) {
        furi_delay_ms(50);
    }

    furi_hal_nfc_field_off();
    furi_hal_nfc_sleep();
}

static uint8_t hexdigit(char hex) {
    return (hex <= '9') ? hex - '0' : toupper(hex) - 'A' + 10 ;
}

static uint8_t hexbyte(const char* hex) {
    return (hexdigit(*hex) << 4) | hexdigit(*(hex+1));
}

static void nfc_cli_st25r_write(Cli* cli, FuriString* args) {
    UNUSED(cli);
    FuriString* reg = furi_string_alloc();
    FuriString* value = furi_string_alloc();

    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_busy()) {
        printf("Nfc is busy\r\n");
        return;
    }

    if(!args_read_string_and_trim(args, reg) || !args_read_string_and_trim(args, value)) {
        printf("You didn't specify <reg> and <value>\r\n");
        nfc_cli_print_usage();
        return;
    }

    if(furi_string_utf8_length(reg) != 2 || furi_string_utf8_length(value) != 2) {
        printf("You didn't specify <reg> and <value> as 2-character hex values\r\n");
        nfc_cli_print_usage();
        return;
    }

    uint8_t reg_val = hexbyte(furi_string_get_cstr(reg));
    uint8_t value_val = hexbyte(furi_string_get_cstr(value));

    printf("W %02X <- %02X\r\n", reg_val, value_val);
    if(st25r3916WriteRegister(reg_val, value_val) != ERR_NONE) {
        printf("Failed to write register\r\n");
        return;
    }
}

static void nfc_cli_st25r_read(Cli* cli, FuriString* args) {
    UNUSED(cli);
    FuriString* reg = furi_string_alloc();

    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_busy()) {
        printf("Nfc is busy\r\n");
        return;
    }

    if(!args_read_string_and_trim(args, reg)) {
        printf("You didn't specify <reg>\r\n");
        nfc_cli_print_usage();
        return;
    }

    if(furi_string_utf8_length(reg) != 2) {
        printf("You didn't specify <reg> as 2-character hex value\r\n");
        nfc_cli_print_usage();
        return;
    }

    uint8_t reg_val = hexbyte(furi_string_get_cstr(reg));
    uint8_t value_val = 0;

    if(st25r3916ReadRegister(reg_val, &value_val) != ERR_NONE) {
        printf("Failed to read register\r\n");
        return;
    }

    printf("R %02X -> %02X\r\n", reg_val, value_val);
}

static void nfc_cli_st25r_cmd(Cli* cli, FuriString* args) {
    UNUSED(cli);
    FuriString* cmd = furi_string_alloc();

    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_busy()) {
        printf("Nfc is busy\r\n");
        return;
    }

    if(!args_read_string_and_trim(args, cmd)) {
        printf("You didn't specify <cmd>\r\n");
        nfc_cli_print_usage();
        return;
    }

    if(furi_string_utf8_length(cmd) != 2) {
        printf("You didn't specify <cmd> as 2-character hex value\r\n");
        nfc_cli_print_usage();
        return;
    }

    uint8_t cmd_val = hexbyte(furi_string_get_cstr(cmd));

    if(st25r3916ExecuteCommand(cmd_val) != ERR_NONE) {
        printf("Failed to execute\r\n");
        return;
    }

    printf("X %02X\r\n", cmd_val);
}

static FuriHalNfcTxRxContext tx_rx = {};

static void nfc_cli_st25r_fifo(Cli* cli, FuriString* args) {
    UNUSED(args);

    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_busy()) {
        printf("Nfc is busy\r\n");
        return;
    }

    bool field = false;

    tx_rx.sniff_tx = NULL;
    tx_rx.sniff_rx = NULL;
    tx_rx.tx_bits = 0;
    tx_rx.tx_rx_type = FuriHalNfcTxRxTypeRxRaw;
    
    rfal_platform_spi_acquire();

    furi_hal_nfcv_listen_start();

    printf("Reading FIFO...\r\nPress Ctrl+C to abort\r\n");
    while(!cli_cmd_interrupt_received(cli)) {

        if(st25r3916IsExtFieldOn() && !field) {
            printf("Field entered\r\n");
            field = true;
        }
        if(!st25r3916IsExtFieldOn() && field) {
            printf("Field left\r\n");
            field = false;
        }

        if(furi_hal_nfc_listen_rx(&tx_rx, 50)) {
            for(int pos = 0; pos < tx_rx.rx_bits / 8; pos++) {
                printf(" %02X", tx_rx.rx_data[pos]);
            }
            printf("\r\n");
        }

        furi_delay_ms(50);
    }
    rfal_platform_spi_release();
}


static void nfc_cli_st25r_trans(Cli* cli, FuriString* args) {
    UNUSED(args);
    
    // Check if nfc worker is not busy
    if(furi_hal_nfc_is_busy()) {
        printf("Nfc is busy\r\n");
        return;
    }
    
    printf("ISO15693 emulator...\r\nPress Ctrl+C to abort\r\n");

    FuriHalNfcDevData nfc_data = {
        .uid = { 0x36, 0x78, 0x45, 0x0E, 0x50, 0x03, 0x04, 0xE0 },
        .uid_len = 8,
        .type = FuriHalNfcTypeV,
    };
    NfcVData nfcv_data = {
        .afi = 0,
        .dsfid = 0,
        .block_num = 8,
        .block_size = 4,
        .ic_ref = 3,
        .type = NfcVTypeSlixL,
        .sub_data.slix_l = {
            .key_privacy = { 0x0F, 0x0F, 0x0F, 0x0F },
            .privacy = false
        }
    };

    memset(nfcv_data.data, 0xAE, 4 * 8);

    nfcv_emu_init();
    while(!cli_cmd_interrupt_received(cli)) {
        if(nfcv_emu_loop(&nfc_data, &nfcv_data, 1000)) {
            printf("[NfcV-Emu] %s\r\n", nfcv_data.last_command);
        }
    }

    nfcv_emu_deinit();
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
        if(furi_string_cmp_str(cmd, "emulate") == 0) {
            nfc_cli_emulate(cli, args);
            break;
        }

        if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
            if(furi_string_cmp_str(cmd, "field") == 0) {
                nfc_cli_field(cli, args);
                break;
            }
            if(furi_string_cmp_str(cmd, "st25r_read") == 0) {
                nfc_cli_st25r_read(cli, args);
                break;
            }
            if(furi_string_cmp_str(cmd, "st25r_write") == 0) {
                nfc_cli_st25r_write(cli, args);
                break;
            }
            if(furi_string_cmp_str(cmd, "st25r_cmd") == 0) {
                nfc_cli_st25r_cmd(cli, args);
                break;
            }
            if(furi_string_cmp_str(cmd, "st25r_fifo") == 0) {
                nfc_cli_st25r_fifo(cli, args);
                break;
            }
            if(furi_string_cmp_str(cmd, "st25r_trans") == 0) {
                nfc_cli_st25r_trans(cli, args);
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
