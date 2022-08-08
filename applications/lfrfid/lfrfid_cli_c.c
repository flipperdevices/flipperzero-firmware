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
    printf("\t<key_type> choose from:\r\n");
    printf("\tEM4100, EM-Marin (5 bytes key_data)\r\n");
    printf("\tH10301, HID26 (3 bytes key_data)\r\n");
    printf("\tI40134, Indala (3 bytes key_data)\r\n");
    printf("\t<key_data> are hex-formatted\r\n");
    printf("rfid raw_read <ask | psk> <filename>\r\n");
    printf("rfid raw_emulate <filename>\r\n");
};

typedef struct {
    ProtocolId protocol;
    osEventFlagsId_t event;
} LFRFIDCliReadContext;

static void lfrfid_cli_read_callback(LFRFIDWorkerReadResult result, ProtocolId proto, void* ctx) {
    furi_assert(ctx);
    LFRFIDCliReadContext* context = ctx;
    if(result == LFRFIDWorkerReadDone) {
        context->protocol = proto;
        FURI_SW_MEMBARRIER();
    }
    osEventFlagsSet(context->event, 1 << result);
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

    LFRFIDWorker* worker = lfrfid_worker_alloc();
    LFRFIDCliReadContext context;
    context.protocol = PROTOCOL_NO;
    context.event = osEventFlagsNew(NULL);

    lfrfid_worker_start_thread(worker);
    lfrfid_worker_read_set_callback(worker, lfrfid_cli_read_callback, &context);

    printf("Reading RFID...\r\nPress Ctrl+C to abort\r\n");

    const uint32_t available_flags = (1 << LFRFIDWorkerReadSenseStart) |
                                     (1 << LFRFIDWorkerReadSenseEnd) | (1 << LFRFIDWorkerReadDone);

    lfrfid_worker_read_start(worker, type);

    while(true) {
        uint32_t flags = osEventFlagsWait(context.event, available_flags, osFlagsWaitAny, 100);

        if(flags != osFlagsErrorTimeout) {
            if(FURI_BIT(flags, LFRFIDWorkerReadSenseStart)) {
                printf("Sense Start\r\n");
            }

            if(FURI_BIT(flags, LFRFIDWorkerReadSenseEnd)) {
                printf("Sense End\r\n");
            }

            if(FURI_BIT(flags, LFRFIDWorkerReadDone)) {
                break;
            }
        }

        if(cli_cmd_interrupt_received(cli)) break;
    }

    lfrfid_worker_stop(worker);

    if(context.protocol != PROTOCOL_NO) {
        printf("%s ", lfrfid_worker_dict_get_name(worker, context.protocol));

        size_t size = lfrfid_worker_dict_get_data_size(worker, context.protocol);
        uint8_t* data = malloc(size);
        lfrfid_worker_dict_get_data(worker, context.protocol, data, size);
        for(size_t i = 0; i < size; i++) {
            printf("%02X", data[i]);
        }
        printf("\r\n");
        free(data);

        string_t info;
        string_init(info);
        lfrfid_worker_dict_render(worker, context.protocol, info);
        if(string_size(info) > 0) {
            printf("%s\r\n", string_get_cstr(info));
        }
        string_clear(info);
    }

    printf("Reading stopped\r\n");
    lfrfid_worker_stop_thread(worker);
    lfrfid_worker_free(worker);

    osEventFlagsDelete(context.event);
}

static void lfrfid_cli_write(Cli* cli, string_t args) {
    UNUSED(cli);
    UNUSED(args);
    // TODO implement rfid write
    printf("Not implemented :(\r\n");
}

static void lfrfid_cli_emulate(Cli* cli, string_t args) {
    UNUSED(cli);
    UNUSED(args);
    // TODO implement rfid emulate
    printf("Not implemented :(\r\n");
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

static void lfrfid_cli_read_raw_callback(LFRFIDWorkerReadRawResult result, void* context) {
    furi_assert(context);
    osEventFlagsId_t event = context;
    osEventFlagsSet(event, 1 << result);
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

        LFRFIDWorker* worker = lfrfid_worker_alloc();
        osEventFlagsId_t event = osEventFlagsNew(NULL);

        lfrfid_worker_start_thread(worker);
        lfrfid_worker_read_raw_set_callback(worker, lfrfid_cli_read_raw_callback, event);

        bool overrun = false;

        const uint32_t available_flags = (1 << LFRFIDWorkerReadRawFileError) |
                                         (1 << LFRFIDWorkerReadRawOverrun) |
                                         (1 << LFRFIDWorkerReadRawDone);

        lfrfid_worker_read_raw_start(worker, string_get_cstr(filepath), type);
        while(true) {
            uint32_t flags = osEventFlagsWait(event, available_flags, osFlagsWaitAny, 100);

            if(flags != osFlagsErrorTimeout) {
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

        osEventFlagsDelete(event);

    } while(false);

    string_clear(filepath);
    string_clear(type_string);
}

static void
    lfrfid_worker_emulate_raw_callback(LFRFIDWorkerEmulateRawResult result, void* context) {
    furi_assert(context);
    osEventFlagsId_t event = context;
    osEventFlagsSet(event, 1 << result);
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

        LFRFIDWorker* worker = lfrfid_worker_alloc();
        osEventFlagsId_t event = osEventFlagsNew(NULL);

        lfrfid_worker_start_thread(worker);
        lfrfid_worker_emulate_raw_set_callback(worker, lfrfid_worker_emulate_raw_callback, event);

        bool overrun = false;

        const uint32_t available_flags = (1 << LFRFIDWorkerEmulateRawFileError) |
                                         (1 << LFRFIDWorkerEmulateRawOverrun) |
                                         (1 << LFRFIDWorkerEmulateRawDone);

        lfrfid_worker_emulate_raw_start(worker, string_get_cstr(filepath));
        while(true) {
            uint32_t flags = osEventFlagsWait(event, available_flags, osFlagsWaitAny, 100);

            if(flags != osFlagsErrorTimeout) {
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

        osEventFlagsDelete(event);

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