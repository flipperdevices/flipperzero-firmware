#include <furi.h>
#include <cli/cli.h>
#include "music_player_worker.h"

static void music_player_cli(Cli* cli, string_t args, void* context) {
    MusicPlayerWorker* music_player_worker = music_player_worker_alloc();

    do {
        if(!music_player_worker_load(music_player_worker, string_get_cstr(args))) {
            printf("Failed to open file %s", string_get_cstr(args));
            break;
        }

        printf("Press CTRL+C to stop");
        music_player_worker_start(music_player_worker);
        while(!cli_cmd_interrupt_received(cli)) {
            osDelay(50);
        }
        music_player_worker_stop(music_player_worker);
    } while(0);

    music_player_worker_free(music_player_worker);
}

void music_player_on_system_start() {
#ifdef SRV_CLI
    Cli* cli = furi_record_open("cli");

    cli_add_command(cli, "music_player", CliCommandFlagDefault, music_player_cli, NULL);

    furi_record_close("cli");
#else
    UNUSED(music_player_cli);
#endif
}
