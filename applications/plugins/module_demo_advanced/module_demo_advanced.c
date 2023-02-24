#include "module_api.h"
#include "app_methods.h"

#include "app_api_table.h"

#include <flipper_application/flipper_application.h>
#include <flipper_application/application_library/application_lib_manager.h>
#include <flipper_application/application_library/composite_resolver.h>

#include <loader/firmware_api/firmware_api.h>

#define TAG "module_demo_advanced"

int32_t module_demo_advanced_app(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "I'm module_demo_advanced!");

    CompositeApiResolver* resolver = composite_api_resolver_alloc();
    composite_api_resolver_add(resolver, firmware_api_interface);
    composite_api_resolver_add(resolver, module_demo_api_interface);

    ApplicationLibManager* manager = application_lib_manager_alloc(
        APP_ID, APP_MODULE_API_VERSION, composite_api_resolver_get(resolver));

    if(application_lib_manager_load_all(
           manager, "/ext/apps_data/module_demo_advanced/extensions") !=
       ApplicationLibManagerErrorNone) {
        FURI_LOG_E(TAG, "Failed to load all libs");
        return 0;
    }

    uint32_t module_count = application_lib_manager_get_count(manager);
    FURI_LOG_I(TAG, "Loaded libs: %lu", module_count);

    for(uint32_t i = 0; i < module_count; i++) {
        const AdvancedDemoAppModule* module = application_lib_manager_get_ep(manager, i);
        FURI_LOG_I(TAG, "module name: %s. Calling methods", module->name);
        module->method1(228);
        module->method2();
        FURI_LOG_I(TAG, "Accumulator: %lu", app_api_accumulator_get());
    }

    application_lib_manager_free(manager);
    composite_api_resolver_free(resolver);
    FURI_LOG_I(TAG, "Goodbye!");

    return 0;
}
