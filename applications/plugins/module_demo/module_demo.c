#include <furi.h>

#include <flipper_application/flipper_application.h>
#include <loader/firmware_api/firmware_api.h>
#include <storage/storage.h>

#include "demo_module_api.h"

#define TAG "module_demo"

int32_t module_demo_app(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "I'm module_demo!");

    Storage* storage = furi_record_open(RECORD_STORAGE);

    FlipperApplication* app = flipper_application_alloc(storage, &firmware_api_interface);

    do {
        FlipperApplicationPreloadStatus preload_res = flipper_application_preload(
            app, "/ext/apps_data/module_demo/extensions/module_demo_plugin1.fal");

        if(preload_res != FlipperApplicationPreloadStatusSuccess) {
            FURI_LOG_E(TAG, "Failed to preload plugin");
            break;
        }

        if(!flipper_application_is_lib(app)) {
            FURI_LOG_E(TAG, "Plugin file is not a library");
            break;
        }

        FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(app);
        if(load_status != FlipperApplicationLoadStatusSuccess) {
            FURI_LOG_E(TAG, "Failed to load plugin file");
            break;
        }

        const FlipperApplicationLibraryDescriptor* app_descriptor =
            flipper_application_lib_get(app);

        const DemoAppModule* module = app_descriptor->entry_point;

        FURI_LOG_I(TAG, "Plugin name: %s", module->name);
        FURI_LOG_I(TAG, "Plugin method1: %d", module->method1());
        FURI_LOG_I(TAG, "Plugin method2(7,8): %d", module->method2(7, 8));
    } while(false);
    flipper_application_free(app);

    furi_record_close(RECORD_STORAGE);
    FURI_LOG_I(TAG, "Goodbye!");

    return 0;
}
