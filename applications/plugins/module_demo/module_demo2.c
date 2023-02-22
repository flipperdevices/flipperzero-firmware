#include <furi.h>

#include <flipper_application/flipper_application.h>
#include <flipper_application/app_lib/app_lib_manager.h>
#include <loader/firmware_api/firmware_api.h>
#include <storage/storage.h>

#include "demo_module_api.h"

#define TAG "module_demo2"

int32_t module_demo_app2(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "I'm module_demo2!");

    ApplicationLibManager* manager = application_lib_manager_alloc(
        DEMO_APP_ID, DEMO_APP_MODULE_API_VERSION, &firmware_api_interface);

    if(application_lib_manager_load_all(manager, "/ext/apps_data/module_demo/extensions") !=
       ApplicationLibManagerErrorNone) {
        FURI_LOG_E(TAG, "Failed to load all libs");
        return 0;
    }

    uint32_t module_count = application_lib_manager_get_count(manager);
    FURI_LOG_I(TAG, "Loaded libs: %ld", module_count);

    for(uint32_t i = 0; i < module_count; i++) {
        const DemoAppModule* module = application_lib_manager_get_ep(manager, i);
        FURI_LOG_I(TAG, "module name: %s", module->name);
        FURI_LOG_I(TAG, "module method1: %d", module->method1());
        FURI_LOG_I(TAG, "module method2(7,8): %d", module->method2(7, 8));
    }

    application_lib_manager_free(manager);
    FURI_LOG_I(TAG, "Goodbye!");

    return 0;
}
