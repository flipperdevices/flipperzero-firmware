#include <furi.h>
#include <furi_hal.h>
#include <stdarg.h>
#include <cli/cli.h>
#include <lib/one_wire/ibutton/ibutton_worker.h>

#include <memory>

void ibutton_cli_qr(Cli* cli, string_t args, void* context);
void ibutton_cli_qw(Cli* cli, string_t args, void* context);
void ibutton_cli_qe(Cli* cli, string_t args, void* context);

// app cli function
extern "C" void ibutton_cli_w_init() {
    Cli* cli = static_cast<Cli*>(furi_record_open("cli"));
    cli_add_command(cli, "qr", CliCommandFlagDefault, ibutton_cli_qr, cli);
    cli_add_command(cli, "qw", CliCommandFlagDefault, ibutton_cli_qw, cli);
    cli_add_command(cli, "qe", CliCommandFlagDefault, ibutton_cli_qe, cli);
}

void ibutton_cli_worker_read_cb(void* context) {
    osEventFlagsId_t event = context;
    osEventFlagsSet(event, 1);
}

void ibutton_cli_qr(Cli* cli, string_t args, void* context) {
    (void)args;
    (void)context;

    osEventFlagsId_t event = osEventFlagsNew(NULL);

    iButtonKey* key = ibutton_key_alloc();
    iButtonWorker* worker = ibutton_worker_alloc();
    ibutton_worker_start_thread(worker);
    ibutton_worker_read_set_callback(worker, ibutton_cli_worker_read_cb, event);
    ibutton_worker_read_start(worker, key);

    osEventFlagsWait(event, 1, osFlagsWaitAny, osWaitForever);
    osEventFlagsDelete(event);

    const uint8_t* key_data = ibutton_key_get_data_p(key);
    switch(ibutton_key_get_type(key)) {
    case iButtonKeyDS1990:
        printf(
            "Dallas %02X%02X%02X%02X%02X%02X%02X%02X\r\n",
            key_data[0],
            key_data[1],
            key_data[2],
            key_data[3],
            key_data[4],
            key_data[5],
            key_data[6],
            key_data[7]);
        break;
    case iButtonKeyCyfral:
        printf("Cyfral %02X%02X\r\n", key_data[0], key_data[1]);
        break;
    case iButtonKeyMetakom:
        printf("Metakom %02X%02X%02X%02X\r\n", key_data[0], key_data[1], key_data[2], key_data[3]);
        break;
    }

    ibutton_worker_stop(worker);
    ibutton_worker_stop_thread(worker);
    ibutton_worker_free(worker);
    ibutton_key_free(key);
}

void ibutton_cli_worker_write_cb(void* context, iButtonWorkerWriteResult result) {
    osEventFlagsId_t event = context;
    osEventFlagsSet(event, (1 << (uint32_t)result));
}

void ibutton_cli_qw(Cli* cli, string_t args, void* context) {
    (void)args;
    (void)context;

    osEventFlagsId_t event = osEventFlagsNew(NULL);

    iButtonKey* key = ibutton_key_alloc();
    iButtonWorker* worker = ibutton_worker_alloc();

    uint8_t key_data[8] = {
        0x01,
        0x02,
        0x03,
        0x04,
        0xA0,
        0xB0,
        0xC0,
        0xD0,
    };
    ibutton_key_set_type(key, iButtonKeyDS1990);
    ibutton_key_set_data(key, key_data, 8);

    ibutton_worker_start_thread(worker);
    ibutton_worker_write_set_callback(worker, ibutton_cli_worker_write_cb, event);
    ibutton_worker_write_start(worker, key);

    while(1) {
        uint32_t value = osEventFlagsWait(event, 0b1111, osFlagsWaitAny, osWaitForever);
        if(value & 0b0001) printf("WriteOK\r\n");
        if(value & 0b0010) printf("SameKey\r\n");
        if(value & 0b0100) printf("NoDetect\r\n");
        if(value & 0b1000) printf("CannotWrite\r\n");
        delay(100);
    }

    osEventFlagsDelete(event);

    ibutton_worker_stop(worker);
    ibutton_worker_stop_thread(worker);
    ibutton_worker_free(worker);
    ibutton_key_free(key);
}

void ibutton_cli_worker_emulate_cb(void* context, bool emulated) {
    osEventFlagsId_t event = context;
    osEventFlagsSet(event, 1);
}

void ibutton_cli_qe(Cli* cli, string_t args, void* context) {
    (void)args;
    (void)context;

    osEventFlagsId_t event = osEventFlagsNew(NULL);

    iButtonKey* key = ibutton_key_alloc();
    iButtonWorker* worker = ibutton_worker_alloc();

    uint8_t key_data[8] = {
        0xA2,
        0xD6,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
    ibutton_key_set_type(key, iButtonKeyCyfral);
    ibutton_key_set_data(key, key_data, 8);

    ibutton_worker_start_thread(worker);
    ibutton_worker_emulate_set_callback(worker, ibutton_cli_worker_emulate_cb, event);
    ibutton_worker_emulate_start(worker, key);

    while(1) {
        osEventFlagsWait(event, 1, osFlagsWaitAny, osWaitForever);
        printf("Emulated\r\n");
        delay(100);
    }

    osEventFlagsDelete(event);

    ibutton_worker_stop(worker);
    ibutton_worker_stop_thread(worker);
    ibutton_worker_free(worker);
    ibutton_key_free(key);
}