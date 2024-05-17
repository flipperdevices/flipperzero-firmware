#include "subghz_gps.h"

#include <expansion/expansion.h>
#include <loader/firmware_api/firmware_api.h>
#include <inttypes.h>

#define TAG "SubGhzGPS"

SubGhzGPS* subghz_gps_plugin_init(uint32_t baudrate) {
    bool connected = expansion_is_connected(furi_record_open(RECORD_EXPANSION));
    furi_record_close(RECORD_EXPANSION);
    if(connected) return NULL;

    expansion_disable(furi_record_open(RECORD_EXPANSION));
    furi_record_close(RECORD_EXPANSION);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperApplication* plugin_app = flipper_application_alloc(storage, firmware_api_interface);
    do {
        FlipperApplicationPreloadStatus preload_res = flipper_application_preload(
            plugin_app, EXT_PATH("apps_data/subghz_gps/plugins/subghz_gps.fal"));

        if(preload_res != FlipperApplicationPreloadStatusSuccess) {
            FURI_LOG_E(TAG, "Failed to preload GPS plugin. Code: %d\r\n", preload_res);
            break;
        }

        if(!flipper_application_is_plugin(plugin_app)) {
            FURI_LOG_E(TAG, "GPS plugin file is not a library\r\n");
            break;
        }

        FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(plugin_app);
        if(load_status != FlipperApplicationLoadStatusSuccess) {
            FURI_LOG_E(TAG, "Failed to load GPS plugin file. Code %d\r\n", load_status);
            break;
        }

        const FlipperAppPluginDescriptor* app_descriptor =
            flipper_application_plugin_get_descriptor(plugin_app);

        if(strcmp(app_descriptor->appid, "subghz_gps") != 0) {
            FURI_LOG_E(TAG, "GPS plugin type doesn't match\r\n");
            break;
        }

        if(app_descriptor->ep_api_version != 1) {
            FURI_LOG_E(
                TAG,
                "GPS plugin version %" PRIu32 " doesn't match\r\n",
                app_descriptor->ep_api_version);
            break;
        }

        void (*subghz_gps_init)(SubGhzGPS* subghz_gps, uint32_t baudrate) =
            app_descriptor->entry_point;

        SubGhzGPS* subghz_gps = malloc(sizeof(SubGhzGPS));
        subghz_gps->plugin_app = plugin_app;
        subghz_gps_init(subghz_gps, baudrate);
        return subghz_gps;

    } while(false);
    flipper_application_free(plugin_app);
    furi_record_close(RECORD_STORAGE);

    expansion_enable(furi_record_open(RECORD_EXPANSION));
    furi_record_close(RECORD_EXPANSION);
    return NULL;
}

void subghz_gps_plugin_deinit(SubGhzGPS* subghz_gps) {
    subghz_gps->deinit(subghz_gps);
    flipper_application_free(subghz_gps->plugin_app);
    free(subghz_gps);
    furi_record_close(RECORD_STORAGE);

    expansion_enable(furi_record_open(RECORD_EXPANSION));
    furi_record_close(RECORD_EXPANSION);
}
