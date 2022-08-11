#include <furi.h>
#include <furi_hal.h>
#include <stdarg.h>
#include <cli/cli.h>
#include <lib/toolbox/args.h>
#include <lib/lfrfid/lfrfid_worker.h>
#include <storage/storage.h>
#include <toolbox/stream/file_stream.h>

#include <toolbox/varint.h>

#include <toolbox/protocols/protocol_dict.h>
#include <lfrfid/protocols/lfrfid_protocols.h>
#include <toolbox/pulse_protocols/pulse_glue.h>

#define RAW_HEADER "RFID RAW"

#define FURI_SW_MEMBARRIER() asm volatile("" : : : "memory")

static void lfrfid_cli(Cli* cli, string_t args, void* context);

// app cli function
void lfrfid_c_on_system_start() {
    Cli* cli = furi_record_open("cli");
    cli_add_command(cli, "rfid", CliCommandFlagDefault, lfrfid_cli, NULL);
    furi_record_close("cli");
}

static void lfrfid_cli_print_usage() {
    printf("Usage:\r\n");
    printf("rfid read <optional: normal | indala>\r\n");
    printf("rfid <write | emulate> <key_type> <key_data>\r\n");
    printf("rfid raw_read <ask | psk> <filename>\r\n");
    printf("rfid raw_emulate <filename>\r\n");
};

typedef struct {
    ProtocolId protocol;
    FuriEventFlag* event;
} LFRFIDCliReadContext;

static void lfrfid_cli_read_callback(LFRFIDWorkerReadResult result, ProtocolId proto, void* ctx) {
    furi_assert(ctx);
    LFRFIDCliReadContext* context = ctx;
    if(result == LFRFIDWorkerReadDone) {
        context->protocol = proto;
        FURI_SW_MEMBARRIER();
    }
    furi_event_flag_set(context->event, 1 << result);
}

static void lfrfid_cli_read(Cli* cli, string_t args) {
    string_t type_string;
    string_init(type_string);
    LFRFIDWorkerReadType type = LFRFIDWorkerReadTypeAuto;

    if(args_read_string_and_trim(args, type_string)) {
        if(string_cmp_str(type_string, "normal") == 0 || string_cmp_str(type_string, "ask") == 0) {
            // ask
            type = LFRFIDWorkerReadTypeASKOnly;
        } else if(
            string_cmp_str(type_string, "indala") == 0 ||
            string_cmp_str(type_string, "psk") == 0) {
            // psk
            type = LFRFIDWorkerReadTypePSKOnly;
        } else {
            lfrfid_cli_print_usage();
            string_clear(type_string);
            return;
        }
    }
    string_clear(type_string);

    ProtocolDict* dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
    LFRFIDWorker* worker = lfrfid_worker_alloc(dict);
    LFRFIDCliReadContext context;
    context.protocol = PROTOCOL_NO;
    context.event = furi_event_flag_alloc();

    lfrfid_worker_start_thread(worker);
    lfrfid_worker_read_set_callback(worker, lfrfid_cli_read_callback, &context);

    printf("Reading RFID...\r\nPress Ctrl+C to abort\r\n");

    const uint32_t available_flags = (1 << LFRFIDWorkerReadDone);

    lfrfid_worker_read_start(worker, type);

    while(true) {
        uint32_t flags =
            furi_event_flag_wait(context.event, available_flags, FuriFlagWaitAny, 100);

        if(flags != FuriFlagErrorTimeout) {
            if(FURI_BIT(flags, LFRFIDWorkerReadDone)) {
                break;
            }
        }

        if(cli_cmd_interrupt_received(cli)) break;
    }

    lfrfid_worker_stop(worker);
    lfrfid_worker_stop_thread(worker);
    lfrfid_worker_free(worker);

    if(context.protocol != PROTOCOL_NO) {
        printf("%s ", protocol_dict_get_name(dict, context.protocol));

        size_t size = protocol_dict_get_data_size(dict, context.protocol);
        uint8_t* data = malloc(size);
        protocol_dict_get_data(dict, context.protocol, data, size);
        for(size_t i = 0; i < size; i++) {
            printf("%02X", data[i]);
        }
        printf("\r\n");
        free(data);

        string_t info;
        string_init(info);
        protocol_dict_render_data(dict, info, context.protocol);
        if(string_size(info) > 0) {
            printf("%s\r\n", string_get_cstr(info));
        }
        string_clear(info);
    }

    printf("Reading stopped\r\n");
    protocol_dict_free(dict);

    furi_event_flag_free(context.event);
}

static void lfrfid_cli_write_callback(LFRFIDWorkerWriteResult result, void* ctx) {
    furi_assert(ctx);
    FuriEventFlag* events = ctx;
    furi_event_flag_set(events, 1 << result);
}

static bool lfrfid_cli_parse_args(string_t args, ProtocolDict* dict, ProtocolId* protocol) {
    bool result = false;
    string_t protocol_name, data_text;
    string_init(protocol_name);
    string_init(data_text);
    size_t data_size = protocol_dict_get_max_data_size(dict);
    uint8_t* data = malloc(data_size);

    do {
        // load args
        if(!args_read_string_and_trim(args, protocol_name) ||
           !args_read_string_and_trim(args, data_text)) {
            lfrfid_cli_print_usage();
            break;
        }

        // check protocol arg
        *protocol = protocol_dict_get_protocol_by_name(dict, string_get_cstr(protocol_name));
        if(*protocol == PROTOCOL_NO) {
            printf(
                "Unknown protocol: %s\r\n"
                "Available protocols:\r\n",
                string_get_cstr(protocol_name));

            for(ProtocolId i = 0; i < LFRFIDProtocolMax; i++) {
                printf(
                    "\t%s, %d bytes long\r\n",
                    protocol_dict_get_name(dict, i),
                    protocol_dict_get_data_size(dict, i));
            }
            break;
        }

        data_size = protocol_dict_get_data_size(dict, *protocol);

        // check data arg
        if(!args_read_hex_bytes(data_text, data, data_size)) {
            printf(
                "%s data needs to be %d bytes long\r\n",
                protocol_dict_get_name(dict, *protocol),
                data_size);
            break;
        }

        // load data to protocol
        protocol_dict_set_data(dict, *protocol, data, data_size);

        result = true;
    } while(false);

    free(data);
    string_clear(protocol_name);
    string_clear(data_text);
    return result;
}

static void lfrfid_cli_write(Cli* cli, string_t args) {
    ProtocolDict* dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
    ProtocolId protocol;

    if(!lfrfid_cli_parse_args(args, dict, &protocol)) {
        protocol_dict_free(dict);
        return;
    }

    LFRFIDWorker* worker = lfrfid_worker_alloc(dict);
    FuriEventFlag* event = furi_event_flag_alloc();

    lfrfid_worker_start_thread(worker);
    lfrfid_worker_write_set_callback(worker, lfrfid_cli_write_callback, event);
    lfrfid_worker_write_start(worker, protocol);

    printf("Writing RFID...\r\nPress Ctrl+C to abort\r\n");
    const uint32_t available_flags = (1 << LFRFIDWorkerWriteOK) |
                                     (1 << LFRFIDWorkerWriteProtocolCannotBeWritten) |
                                     (1 << LFRFIDWorkerWriteFobCannotBeWritten);

    while(!cli_cmd_interrupt_received(cli)) {
        uint32_t flags = furi_event_flag_wait(event, available_flags, FuriFlagWaitAny, 100);
        if(flags != FuriFlagErrorTimeout) {
            if(FURI_BIT(flags, LFRFIDWorkerWriteOK)) {
                printf("Written!\r\n");
                break;
            }

            if(FURI_BIT(flags, LFRFIDWorkerWriteProtocolCannotBeWritten)) {
                printf("This protocol cannot be written.\r\n");
                break;
            }

            if(FURI_BIT(flags, LFRFIDWorkerWriteFobCannotBeWritten)) {
                printf("Seems this fob cannot be written.\r\n");
            }
        }
    }
    printf("Writing stopped\r\n");

    lfrfid_worker_stop(worker);
    lfrfid_worker_stop_thread(worker);
    lfrfid_worker_free(worker);
    protocol_dict_free(dict);
    furi_event_flag_free(event);
}

static void lfrfid_cli_emulate(Cli* cli, string_t args) {
    ProtocolDict* dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
    ProtocolId protocol;

    if(!lfrfid_cli_parse_args(args, dict, &protocol)) {
        protocol_dict_free(dict);
        return;
    }

    LFRFIDWorker* worker = lfrfid_worker_alloc(dict);

    lfrfid_worker_start_thread(worker);
    lfrfid_worker_emulate_start(worker, protocol);

    printf("Emulating RFID...\r\nPress Ctrl+C to abort\r\n");
    while(!cli_cmd_interrupt_received(cli)) {
        furi_delay_ms(100);
    }
    printf("Emulation stopped\r\n");

    lfrfid_worker_stop(worker);
    lfrfid_worker_stop_thread(worker);
    lfrfid_worker_free(worker);
    protocol_dict_free(dict);
}

static void lfrfid_cli_raw_analyze(Cli* cli, string_t args) {
    UNUSED(cli);
    string_t filepath, info_string;
    string_init(filepath);
    string_init(info_string);
    Storage* storage = furi_record_open("storage");
    Stream* stream = file_stream_alloc(storage);

    do {
        float frequency = 0;
        float duty_cycle = 0;

        if(!args_read_probably_quoted_string_and_trim(args, filepath)) {
            lfrfid_cli_print_usage();
            break;
        }

        if(!file_stream_open(stream, string_get_cstr(filepath), FSAM_READ, FSOM_OPEN_EXISTING)) {
            printf("Failed to open file: %s\r\n", string_get_cstr(filepath));
            break;
        }

        char* header_text = malloc(strlen(RAW_HEADER) + 1);

        size_t size = stream_read(stream, (uint8_t*)header_text, strlen(RAW_HEADER));
        if(size != strlen(RAW_HEADER) || strcmp(header_text, RAW_HEADER) != 0) {
            printf("Invalid header\r\n");
            break;
        }

        free(header_text);

        uint32_t max_buffer_size;
        size = stream_read(stream, (uint8_t*)&max_buffer_size, sizeof(uint32_t));
        if(size != sizeof(uint32_t)) {
            printf("Invalid header\r\n");
            break;
        }

        size = stream_read(stream, (uint8_t*)&frequency, sizeof(float));
        if(size != sizeof(float) || frequency < 0.0f || frequency > 1000000.0f) {
            printf("Invalid header\r\n");
            break;
        }

        size = stream_read(stream, (uint8_t*)&duty_cycle, sizeof(float));
        if(size != sizeof(float) || duty_cycle < 0.0f || duty_cycle > 1.0f) {
            printf("Invalid header\r\n");
            break;
        }

        uint8_t* buffer = malloc(max_buffer_size);

        bool file_valid = true;
        uint32_t total_warns = 0;
        uint32_t total_duration = 0;
        uint32_t total_pulse = 0;
        ProtocolId total_protocol = PROTOCOL_NO;

        ProtocolDict* dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
        protocol_dict_decoders_start(dict);

        while(!stream_eof(stream) && file_valid) {
            size_t buffer_size = 0;
            size_t buffer_counter = 0;

            if(stream_read(stream, (uint8_t*)&buffer_size, sizeof(size_t)) != sizeof(size_t)) {
                printf("Failed to read size\r\n");
                break;
            }

            if(stream_read(stream, buffer, buffer_size) != buffer_size) {
                printf("Failed to read data\r\n");
                break;
            }

            while(buffer_counter < buffer_size && file_valid) {
                uint32_t pulse = 0;
                uint32_t duration = 0;

                buffer_counter += varint_uint32_unpack(
                    &pulse, &buffer[buffer_counter], (size_t)(buffer_size - buffer_counter));

                if(buffer_counter >= buffer_size) {
                    printf("buffer is too small\r\n");
                    file_valid = false;
                    break;
                }

                buffer_counter += varint_uint32_unpack(
                    &duration, &buffer[buffer_counter], (size_t)(buffer_size - buffer_counter));

                bool warn = false;

                if(pulse > duration || pulse <= 0 || duration <= 0) {
                    total_warns += 1;
                    warn = true;
                }

                string_printf(info_string, "[%ld %ld]", pulse, duration);
                printf("%-16s", string_get_cstr(info_string));
                string_printf(info_string, "[%ld %ld]", pulse, duration - pulse);
                printf("%-16s", string_get_cstr(info_string));

                if(warn) {
                    printf(" <<----");
                }

                if(total_protocol == PROTOCOL_NO) {
                    total_protocol = protocol_dict_decoders_feed(dict, true, pulse);
                    if(total_protocol == PROTOCOL_NO) {
                        total_protocol =
                            protocol_dict_decoders_feed(dict, false, duration - pulse);
                    }

                    if(total_protocol != PROTOCOL_NO) {
                        printf(" <FOUND %s>", protocol_dict_get_name(dict, total_protocol));
                    }
                }

                printf("\r\n");

                total_pulse += pulse;
                total_duration += duration;

                if(total_protocol != PROTOCOL_NO) {
                    break;
                }
            }

            if(total_protocol != PROTOCOL_NO) {
                break;
            }
        }

        printf("   Frequency: %f\r\n", (double)frequency);
        printf("  Duty Cycle: %f\r\n", (double)duty_cycle);
        printf("       Warns: %ld\r\n", total_warns);
        printf("   Pulse sum: %ld\r\n", total_pulse);
        printf("Duration sum: %ld\r\n", total_duration);
        printf("     Average: %f\r\n", (double)((float)total_pulse / (float)total_duration));
        printf("    Protocol: ");

        if(total_protocol != PROTOCOL_NO) {
            size_t data_size = protocol_dict_get_data_size(dict, total_protocol);
            uint8_t* data = malloc(data_size);
            protocol_dict_get_data(dict, total_protocol, data, data_size);

            printf("%s [", protocol_dict_get_name(dict, total_protocol));
            for(size_t i = 0; i < data_size; i++) {
                printf("%02X", data[i]);
                if(i < data_size - 1) {
                    printf(" ");
                }
            }
            printf("]\r\n");

            protocol_dict_set_data(dict, total_protocol, data, data_size);
            protocol_dict_encoder_start(dict, total_protocol);

            free(data);
        } else {
            printf("not found\r\n");
        }

        protocol_dict_free(dict);
        free(buffer);

    } while(false);

    stream_free(stream);
    string_clear(filepath);
    string_clear(info_string);
    furi_record_close("storage");
}

static void lfrfid_cli_raw_read_callback(LFRFIDWorkerReadRawResult result, void* context) {
    furi_assert(context);
    FuriEventFlag* event = context;
    furi_event_flag_set(event, 1 << result);
}

static void lfrfid_cli_raw_read(Cli* cli, string_t args) {
    UNUSED(cli);

    string_t filepath, type_string;
    string_init(filepath);
    string_init(type_string);
    LFRFIDWorkerReadType type = LFRFIDWorkerReadTypeAuto;

    do {
        if(args_read_string_and_trim(args, type_string)) {
            if(string_cmp_str(type_string, "normal") == 0 ||
               string_cmp_str(type_string, "ask") == 0) {
                // ask
                type = LFRFIDWorkerReadTypeASKOnly;
            } else if(
                string_cmp_str(type_string, "indala") == 0 ||
                string_cmp_str(type_string, "psk") == 0) {
                // psk
                type = LFRFIDWorkerReadTypePSKOnly;
            } else {
                lfrfid_cli_print_usage();
                break;
            }
        }

        if(!args_read_probably_quoted_string_and_trim(args, filepath)) {
            lfrfid_cli_print_usage();
            break;
        }

        ProtocolDict* dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
        LFRFIDWorker* worker = lfrfid_worker_alloc(dict);
        FuriEventFlag* event = furi_event_flag_alloc();

        lfrfid_worker_start_thread(worker);
        lfrfid_worker_read_raw_set_callback(worker, lfrfid_cli_raw_read_callback, event);

        bool overrun = false;

        const uint32_t available_flags = (1 << LFRFIDWorkerReadRawFileError) |
                                         (1 << LFRFIDWorkerReadRawOverrun) |
                                         (1 << LFRFIDWorkerReadRawDone);

        lfrfid_worker_read_raw_start(worker, string_get_cstr(filepath), type);
        while(true) {
            uint32_t flags = furi_event_flag_wait(event, available_flags, FuriFlagWaitAny, 100);

            if(flags != FuriFlagErrorTimeout) {
                if(FURI_BIT(flags, LFRFIDWorkerReadRawFileError)) {
                    printf("File is not RFID raw file\r\n");
                    break;
                }

                if(FURI_BIT(flags, LFRFIDWorkerReadRawOverrun)) {
                    if(!overrun) {
                        printf("Overrun\r\n");
                        overrun = true;
                    }
                }

                if(FURI_BIT(flags, LFRFIDWorkerReadRawDone)) {
                    // do nothing
                }
            }

            if(cli_cmd_interrupt_received(cli)) break;
        }

        if(overrun) {
            printf("An overrun occurred during read\r\n");
        }

        lfrfid_worker_stop(worker);

        lfrfid_worker_stop_thread(worker);
        lfrfid_worker_free(worker);
        protocol_dict_free(dict);

        furi_event_flag_free(event);

    } while(false);

    string_clear(filepath);
    string_clear(type_string);
}

static void lfrfid_cli_raw_emulate_callback(LFRFIDWorkerEmulateRawResult result, void* context) {
    furi_assert(context);
    FuriEventFlag* event = context;
    furi_event_flag_set(event, 1 << result);
}

static void lfrfid_cli_raw_emulate(Cli* cli, string_t args) {
    UNUSED(cli);

    string_t filepath;
    string_init(filepath);
    Storage* storage = furi_record_open("storage");

    do {
        if(!args_read_probably_quoted_string_and_trim(args, filepath)) {
            lfrfid_cli_print_usage();
            break;
        }

        if(!storage_file_exists(storage, string_get_cstr(filepath))) {
            printf("File not found: \"%s\"\r\n", string_get_cstr(filepath));
            break;
        }

        ProtocolDict* dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
        LFRFIDWorker* worker = lfrfid_worker_alloc(dict);
        FuriEventFlag* event = furi_event_flag_alloc();

        lfrfid_worker_start_thread(worker);
        lfrfid_worker_emulate_raw_set_callback(worker, lfrfid_cli_raw_emulate_callback, event);

        bool overrun = false;

        const uint32_t available_flags = (1 << LFRFIDWorkerEmulateRawFileError) |
                                         (1 << LFRFIDWorkerEmulateRawOverrun) |
                                         (1 << LFRFIDWorkerEmulateRawDone);

        lfrfid_worker_emulate_raw_start(worker, string_get_cstr(filepath));
        while(true) {
            uint32_t flags = furi_event_flag_wait(event, available_flags, FuriFlagWaitAny, 100);

            if(flags != FuriFlagErrorTimeout) {
                if(FURI_BIT(flags, LFRFIDWorkerEmulateRawFileError)) {
                    printf("File is not RFID raw file\r\n");
                    break;
                }

                if(FURI_BIT(flags, LFRFIDWorkerEmulateRawOverrun)) {
                    if(!overrun) {
                        printf("Overrun\r\n");
                        overrun = true;
                    }
                }

                if(FURI_BIT(flags, LFRFIDWorkerEmulateRawDone)) {
                    // do nothing
                }
            }

            if(cli_cmd_interrupt_received(cli)) break;
        }

        if(overrun) {
            printf("An overrun occurred during emulation\r\n");
        }

        lfrfid_worker_stop(worker);

        lfrfid_worker_stop_thread(worker);
        lfrfid_worker_free(worker);
        protocol_dict_free(dict);

        furi_event_flag_free(event);

    } while(false);

    furi_record_close("storage");
    string_clear(filepath);
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
    } else if(string_cmp_str(cmd, "raw_read") == 0) {
        lfrfid_cli_raw_read(cli, args);
    } else if(string_cmp_str(cmd, "raw_emulate") == 0) {
        lfrfid_cli_raw_emulate(cli, args);
    } else if(string_cmp_str(cmd, "raw_analyze") == 0) {
        lfrfid_cli_raw_analyze(cli, args);
    } else {
        lfrfid_cli_print_usage();
    }

    string_clear(cmd);
}