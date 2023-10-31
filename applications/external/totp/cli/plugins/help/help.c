#include <flipper_application/flipper_application.h>
#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>
#include "../../cli_helpers.h"
#include "../../cli_plugin_interface.h"

static void handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    UNUSED(args);
    UNUSED(cli);
    UNUSED(plugin_state);

    Storage* storage = furi_record_open(RECORD_STORAGE);

    Stream* stream = file_stream_alloc(storage);

    if(file_stream_open(
           stream, EXT_PATH("apps_assets/totp/cli/cli_help.txt"), FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint8_t buffer[32U];
        size_t bytes_read;
        while((bytes_read = stream_read(stream, &buffer[0], sizeof(buffer))) > 0) {
            cli_write(cli, &buffer[0], bytes_read);
        }
    }

    file_stream_close(stream);
    stream_free(stream);

    furi_record_close(RECORD_STORAGE);
}

static const CliPlugin plugin = {.name = "TOTP CLI Plugin: Help", .handle = &handle};

static const FlipperAppPluginDescriptor plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &plugin,
};

const FlipperAppPluginDescriptor* totp_cli_help_plugin_ep() {
    return &plugin_descriptor;
}