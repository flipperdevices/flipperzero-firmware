#include "logger_formatter.h"

#include <flipper_application/flipper_application.h>
#include <flipper_application/plugins/plugin_manager.h>
#include <flipper_application/plugins/composite_resolver.h>
#include <loader/firmware_api/firmware_api.h>

#include "../plugins/logger/nfc_logger_formatter_plugin.h"

#define TAG "LogFormatPluginLoader"

void nfc_logger_format(Nfc* nfc, const NfcLoggerFormatterConfig* config) {
    furi_assert(nfc);
    furi_assert(config);

    Storage* storage = furi_record_open(RECORD_STORAGE);

    FlipperApplication* app = flipper_application_alloc(storage, firmware_api_interface);

    do {
        FlipperApplicationPreloadStatus preload_res =
            flipper_application_preload(app, APP_DATA_PATH("plugins/nfc_logger_formatter.fal"));

        if(preload_res != FlipperApplicationPreloadStatusSuccess) {
            FURI_LOG_E(TAG, "Failed to preload plugin");
            break;
        }

        if(!flipper_application_is_plugin(app)) {
            FURI_LOG_E(TAG, "Plugin file is not a library");
            break;
        }

        FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(app);
        if(load_status != FlipperApplicationLoadStatusSuccess) {
            FURI_LOG_E(TAG, "Failed to load plugin file");
            break;
        }

        const FlipperAppPluginDescriptor* app_descriptor =
            flipper_application_plugin_get_descriptor(app);

        furi_check(app_descriptor->ep_api_version == 1);
        furi_check(strcmp(app_descriptor->appid, NFC_LOGGER_FORMATTER_PLUGIN_APP_ID) == 0);

        const NfcLoggerFormatterPlugin* plugin = app_descriptor->entry_point;
        plugin->format(nfc, config);
    } while(false);
    flipper_application_free(app);

    furi_record_close(RECORD_STORAGE);
}
