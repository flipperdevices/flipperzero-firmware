#include "updater_cli.h"

#include "../util/update_manifest.h"

#include <furi.h>
#include <furi_hal.h>
#include <m-string.h>
#include <cli/cli.h>
#include <storage/storage.h>

static void updater_cli_apply(Cli* cli, string_t args, void* context) {
    const char update_dir_path[] = "/ext" UPDATE_DIR_DEFAULT_PATH;
    const char update_manifest_path[] = "/ext" UPDATE_MAINFEST_DEFAULT_PATH;

    printf(
        "Verifying update package at '%s' (manifest '%s')\r\n",
        update_dir_path,
        update_manifest_path);

    UpdateManifest* manifest = update_manifest_alloc();
    if(!update_manifest_init(manifest, update_manifest_path)) {
        printf("Error: Failed to load update manifest\r\n");
    } else {
        printf("Manifest OK.\r\nChecking Stage2 integrity... ");

        Storage* storage = furi_record_open("storage");
        File* file = storage_file_alloc(storage);

        string_t abs_path;
        string_init_printf(
            abs_path, "%s/%s", update_dir_path, string_get_cstr(manifest->staged_loader_file));

        const uint16_t READ_BLOCK = 0x1000;
        uint8_t* read_buffer = malloc(READ_BLOCK);
        uint32_t crc = 0;
        do {
            if(!storage_file_open(file, string_get_cstr(abs_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
                printf("failed to open '%s'. ", string_get_cstr(abs_path));
                break;
            }

            furi_hal_crc_init();
            furi_hal_crc_reset();

            uint16_t bytes_read = 0;
            do {
                bytes_read = storage_file_read(file, read_buffer, READ_BLOCK);
                crc = furi_hal_crc_feed(read_buffer, bytes_read);
            } while(bytes_read == READ_BLOCK);

            furi_hal_crc_deinit();

            printf("CRC %08lx ", crc);
        } while(false);

        string_clear(abs_path);
        free(read_buffer);
        storage_file_free(file);
        furi_record_close("storage");

        if(crc != manifest->staged_loader_crc) {
            printf("MISmatched (ref %08lx). Stopping update.\r\n", manifest->staged_loader_crc);
            update_manifest_free(manifest);
            return;
        }

        printf("OK.\r\nRestarting to apply update. BRB\r\n");
        osDelay(1000);
        furi_hal_rtc_set_flag(FuriHalRtcFlagExecuteUpdate);
        furi_hal_power_reset();
    }
    update_manifest_free(manifest);
}

void updater_on_system_start() {
#ifdef SRV_CLI
    Cli* cli = (Cli*)furi_record_open("cli");
    cli_add_command(cli, "update", CliCommandFlagDefault, updater_cli_apply, NULL);
    furi_record_close("cli");
#else
    (void)&updater_cli_apply;
#endif
}