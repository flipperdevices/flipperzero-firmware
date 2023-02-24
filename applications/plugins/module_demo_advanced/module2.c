#include "app_methods.h"
#include "module_api.h"

#include <flipper_application/flipper_application.h>
#include <furi.h>

static void module_demo_advanced_plugin2_method1(int arg1) {
    app_api_accumulator_mul(arg1);
}

static void module_demo_advanced_plugin2_method2() {
    FURI_LOG_I("TEST", "Plugin 2, accumulator: %lu", app_api_accumulator_get());
}

static const AdvancedDemoAppModule module_demo_advanced_module2 = {
    .name = "Adv Demo App Module 2",
    .method1 = &module_demo_advanced_plugin2_method1,
    .method2 = &module_demo_advanced_plugin2_method2,
};

static const FlipperApplicationLibraryDescriptor module_demo_advanced_module2_descriptor = {
    .appid = APP_ID,
    .ep_api_version = APP_MODULE_API_VERSION,
    .entry_point = &module_demo_advanced_module2,
};

const FlipperApplicationLibraryDescriptor* module_demo_advanced_plugin2_ep() {
    return &module_demo_advanced_module2_descriptor;
}
