#include "../util/update_manifest.h"
#include "../util/tar_archive.h"
#include "../util/lfs_backup.h"
#include "../util/update_hl.h"

#include <furi.h>
#include <furi_hal.h>
#include <m-string.h>
#include <cli/cli.h>
#include <storage/storage.h>
#include <loader/loader.h>
#include <lib/toolbox/path.h>

static void updater_cli_apply(Cli* cli, string_t manifest_path, void* context) {
    printf("Verifying update package at '%s'\r\n", string_get_cstr(manifest_path));

    UpdatePrepareResult result = update_hl_prepare(string_get_cstr(manifest_path));
    if(result != UpdatePrepareResultOK) {
        printf("Error: %s. Stopping update.\r\n", update_hl_describe_preparation_result(result));
        return;
    }
    printf("OK.\r\nRestarting to apply update. BRB\r\n");
    osDelay(100);
    furi_hal_power_reset();
}

static void updater_cli_backup(Cli* cli, string_t args, void* context) {
    printf("Backup\r\n");
    bool success = lfs_backup_unpack(string_get_cstr(args));
    printf("Result = %d\r\n", success);
}

static void updater_cli_restore(Cli* cli, string_t args, void* context) {
    printf("Restore\r\n");
    bool success = lfs_backup_unpack(string_get_cstr(args));
    printf("Result = %d\r\n", success);
}

static int32_t updater_spawner_thread_worker(void* arg) {
    Loader* loader = furi_record_open("loader");
    loader_start(loader, "UpdaterApp", NULL);
    furi_record_close("loader");
    return 0;
}

static void updater_start_app() {
    if(!furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecutePreUpdate) &&
       !furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecutePostUpdate)) {
        return;
    }

    /* We need to spawn a separate thread, because these callbacks are executed 
     * inside loader process, at startup. 
     * So, accessing its record would cause a deadlock 
     */
    FuriThread* thread = furi_thread_alloc();

    furi_thread_set_name(thread, "UpdateAppSpawner");
    furi_thread_set_stack_size(thread, 2048);
    furi_thread_set_callback(thread, updater_spawner_thread_worker);
    furi_thread_start(thread);
}

void updater_on_system_start() {
#ifdef SRV_CLI
    Cli* cli = (Cli*)furi_record_open("cli");
    cli_add_command(cli, "update", CliCommandFlagDefault, updater_cli_apply, NULL);
    cli_add_command(cli, "backup", CliCommandFlagDefault, updater_cli_backup, NULL);
    cli_add_command(cli, "restore", CliCommandFlagDefault, updater_cli_restore, NULL);
    furi_record_close("cli");
#else
    (void)&updater_cli_apply;
    (void)&updater_cli_backup;
    (void)&updater_cli_restore;
#endif
#ifndef FURI_RAM_EXEC
    updater_start_app();
#else
    (void)updater_start_app;
#endif
}
