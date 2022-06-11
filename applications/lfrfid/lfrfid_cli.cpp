#include <furi.h>
#include <furi_hal.h>
#include <stdarg.h>
#include <cli/cli.h>
#include <lib/toolbox/args.h>

#include "helpers/rfid_reader.h"
#include "helpers/rfid_writer.h"
#include "helpers/rfid_timer_emulator.h"

static void lfrfid_cli(Cli* cli, string_t args, void* context);

// app cli function
extern "C" void lfrfid_on_system_start() {
#ifdef SRV_CLI
    Cli* cli = static_cast<Cli*>(furi_record_open("cli"));
    cli_add_command(cli, "rfid", CliCommandFlagDefault, lfrfid_cli, NULL);
    furi_record_close("cli");
#else
    UNUSED(lfrfid_cli);
#endif
}

void lfrfid_cli_print_usage() {
    printf("Usage:\r\n");
    printf("rfid read <optional: normal | indala>\r\n");
    printf("rfid <write | emulate> <key_type> <key_data>\r\n");
    printf("\t<key_type> choose from:\r\n");
    printf("\tEM4100, EM-Marin (5 bytes key_data)\r\n");
    printf("\tH10301, HID26 (3 bytes key_data)\r\n");
    printf("\tI40134, Indala (3 bytes key_data)\r\n");
    printf("\tIoProxXSF, IoProx (4 bytes key_data)\r\n");
    printf("\t<key_data> are hex-formatted\r\n");
};

static bool lfrfid_cli_get_key_type(string_t data, LfrfidKeyType* type) {
    bool result = false;

    if(string_cmp_str(data, "EM4100") == 0 || string_cmp_str(data, "EM-Marin") == 0) {
        result = true;
        *type = LfrfidKeyType::KeyEM4100;
    } else if(string_cmp_str(data, "H10301") == 0 || string_cmp_str(data, "HID26") == 0) {
        result = true;
        *type = LfrfidKeyType::KeyH10301;
    } else if(string_cmp_str(data, "I40134") == 0 || string_cmp_str(data, "Indala") == 0) {
        result = true;
        *type = LfrfidKeyType::KeyI40134;
    } else if(string_cmp_str(data, "IoProxXSF") == 0 || string_cmp_str(data, "IoProx") == 0) {
        result = true;
        *type = LfrfidKeyType::KeyIoProxXSF;
    }

    return result;
}

static void lfrfid_cli_read(Cli* cli, string_t args) {
    RfidReader reader;
    string_t type_string;
    string_init(type_string);
    bool simple_mode = true;
    LfrfidKeyType type;
    RfidReader::Type reader_type = RfidReader::Type::Normal;
    static const uint8_t data_size = LFRFID_KEY_SIZE;
    uint8_t data[data_size] = {0};

    if(args_read_string_and_trim(args, type_string)) {
        simple_mode = false;

        if(string_cmp_str(type_string, "normal") == 0) {
            reader_type = RfidReader::Type::Normal;
        } else if(string_cmp_str(type_string, "indala") == 0) {
            reader_type = RfidReader::Type::Indala;
        } else {
            lfrfid_cli_print_usage();
            string_clear(type_string);
            return;
        }
    }

    if(simple_mode) {
        reader.start();
    } else {
        reader.start_forced(reader_type);
    }

    printf("Reading RFID...\r\nPress Ctrl+C to abort\r\n");
    while(!cli_cmd_interrupt_received(cli)) {
        if(reader.read(&type, data, data_size, simple_mode)) {
            printf("%s", lfrfid_key_get_type_string(type));
            printf(" ");

            for(uint8_t i = 0; i < lfrfid_key_get_type_data_count(type); i++) {
                printf("%02X", data[i]);
            }
            printf("\r\n");
            break;
        }
        furi_hal_delay_ms(100);
    }

    printf("Reading stopped\r\n");
    reader.stop();

    string_clear(type_string);
}

static void lfrfid_cli_write(Cli* cli, string_t args) {
    UNUSED(cli);
    UNUSED(args);
    // TODO implement rfid write
    printf("Not implemented :(\r\n");
}

static void lfrfid_cli_t5577_clear_password_and_config_to_EM(Cli* cli, string_t args) {
    UNUSED(cli);
    UNUSED(args);
    RfidWriter writer;
    const uint32_t default_passwords[] = {
        0x51243648,
        0x000D8787,
        0x19920427,
        0x50524F58,
        0xF9DCEBA0,
        0x65857569,
        0x05D73B9F,
        0x89A69E60,
        0x314159E0,
        0xAA55BBBB,
        0xA5B4C3D2,
        0x1C0B5848,
        0x00434343,
        0x444E4752,
        0x4E457854,
        0x44B44CAE,
        0x88661858,
        0xE9920427,
        0x575F4F4B,
        0x50520901,
        0x20206666,
        0x65857569,
        0x5469616E,
        0x7686962A,
        0xC0F5009A,
        0x07CEE75D,
        0xfeedbeef,
        0xdeadc0de,
        0x00000000,
        0x11111111,
        0x22222222,
        0x33333333,
        0x44444444,
        0x55555555,
        0x66666666,
        0x77777777,
        0x88888888,
        0x99999999,
        0xAAAAAAAA,
        0xBBBBBBBB,
        0xCCCCCCCC,
        0xDDDDDDDD,
        0xEEEEEEEE,
        0xFFFFFFFF,
        0xa0a1a2a3,
        0xb0b1b2b3,
        0x50415353,
        0x00000001,
        0x00000002,
        0x0000000a,
        0x0000000b,
        0x01020304,
        0x02030405,
        0x03040506,
        0x04050607,
        0x05060708,
        0x06070809,
        0x0708090A,
        0x08090A0B,
        0x090A0B0C,
        0x0A0B0C0D,
        0x0B0C0D0E,
        0x0C0D0E0F,
        0x01234567,
        0x12345678,
        0x10000000,
        0x20000000,
        0x30000000,
        0x40000000,
        0x50000000,
        0x60000000,
        0x70000000,
        0x80000000,
        0x90000000,
        0xA0000000,
        0xB0000000,
        0xC0000000,
        0xD0000000,
        0xE0000000,
        0xF0000000,
        0x10101010,
        0x01010101,
        0x11223344,
        0x22334455,
        0x33445566,
        0x44556677,
        0x55667788,
        0x66778899,
        0x778899AA,
        0x8899AABB,
        0x99AABBCC,
        0xAABBCCDD,
        0xBBCCDDEE,
        0xCCDDEEFF,
        0x0CB7E7FC,
        0xFABADA11,
        0x87654321,
        0x12341234,
        0x69696969,
        0x12121212,
        0x12344321,
        0x1234ABCD,
        0x11112222,
        0x13131313,
        0x10041004,
        0x31415926,
        0xabcd1234,
        0x20002000,
        0x19721972,
        0xaa55aa55,
        0x55aa55aa,
        0x4f271149,
        0x07d7bb0b,
        0x9636ef8f,
        0xb5f44686,
        0x9E3779B9,
        0xC6EF3720,
        0x7854794A,
        0xF1EA5EED,
        0x69314718,
        0x57721566,
        0x93C467E3,
        0x27182818,
        0x50415353
    };
    const uint8_t default_passwords_len = sizeof(default_passwords)/sizeof(uint32_t);
    const uint32_t em_config_block_data = 0b00000000000101001000000001000000; //no pwd&aor config block

    printf("Clearing T5577 password (use default passwords) and configuring the tag to default (em-marine)...\r\n");

    for(uint8_t i = 0; i < default_passwords_len; i++) {
        printf("Trying key %u of %u: 0x%08lX\r\n", i+1, default_passwords_len, default_passwords[i]);
        FURI_CRITICAL_ENTER();
        writer.start();
        writer.write_block(0, 0, false, em_config_block_data, true, default_passwords[i]);
        writer.write_reset();
        writer.stop();
        FURI_CRITICAL_EXIT();
        furi_hal_delay_ms(100);
    }

    printf("Done\r\n");
}

static void lfrfid_cli_emulate(Cli* cli, string_t args) {
    string_t data;
    string_init(data);
    RfidTimerEmulator emulator;

    static const uint8_t data_size = LFRFID_KEY_SIZE;
    uint8_t key_data[data_size] = {0};
    uint8_t key_data_size = 0;
    LfrfidKeyType type;

    if(!args_read_string_and_trim(args, data)) {
        lfrfid_cli_print_usage();
        string_clear(data);
        return;
    }

    if(!lfrfid_cli_get_key_type(data, &type)) {
        lfrfid_cli_print_usage();
        string_clear(data);
        return;
    }

    key_data_size = lfrfid_key_get_type_data_count(type);

    if(!args_read_hex_bytes(args, key_data, key_data_size)) {
        lfrfid_cli_print_usage();
        string_clear(data);
        return;
    }

    emulator.start(type, key_data, key_data_size);

    printf("Emulating RFID...\r\nPress Ctrl+C to abort\r\n");
    while(!cli_cmd_interrupt_received(cli)) {
        furi_hal_delay_ms(100);
    }
    printf("Emulation stopped\r\n");
    emulator.stop();

    string_clear(data);
}

static void lfrfid_cli(Cli* cli, string_t args, void* context) {
    UNUSED(context);
    string_t cmd;
    string_init(cmd);

    if(!args_read_string_and_trim(args, cmd)) {
        string_clear(cmd);
        lfrfid_cli_print_usage();
        return;
    }

    if(string_cmp_str(cmd, "read") == 0) {
        lfrfid_cli_read(cli, args);
    } else if(string_cmp_str(cmd, "write") == 0) {
        lfrfid_cli_write(cli, args);
    } else if(string_cmp_str(cmd, "emulate") == 0) {
        lfrfid_cli_emulate(cli, args);
    } else if(string_cmp_str(cmd, "clear_pass_t5577") == 0) {
        lfrfid_cli_t5577_clear_password_and_config_to_EM(cli, args);
    } else {
        lfrfid_cli_print_usage();
    }

    string_clear(cmd);
}
