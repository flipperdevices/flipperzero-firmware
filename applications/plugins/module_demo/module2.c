#include "demo_module_api.h"

#include <flipper_application/flipper_application.h>

static int module_demo_plugin2_method1() {
    return 1337;
}

static int module_demo_plugin2_method2(int arg1, int arg2) {
    return arg1 - arg2;
}

static const DemoAppModule demo_app_module2 = {
    .name = "Demo App Module 2",
    .method1 = &module_demo_plugin2_method1,
    .method2 = &module_demo_plugin2_method2,
};

static const FlipperApplicationLibraryDescriptor demo_app_module2_descriptor = {
    .appid = DEMO_APP_ID,
    .ep_api_version = DEMO_APP_MODULE_API_VERSION,
    .entry_point = &demo_app_module2,
};

const FlipperApplicationLibraryDescriptor* module_demo_plugin2_ep() {
    return &demo_app_module2_descriptor;
}
