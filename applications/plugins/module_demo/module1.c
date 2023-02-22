#include "demo_module_api.h"

#include <flipper_application/flipper_application.h>

static int module_demo_plugin1_method1() {
    return 42;
}

static int module_demo_plugin1_method2(int arg1, int arg2) {
    return arg1 + arg2;
}

static const DemoAppModule demo_app_module1 = {
    .name = "Demo App Module 1",
    .method1 = &module_demo_plugin1_method1,
    .method2 = &module_demo_plugin1_method2,
};

static const FlipperApplicationLibraryDescriptor demo_app_module1_descriptor = {
    .appid = DEMO_APP_ID,
    .ep_api_version = DEMO_APP_MODULE_API_VERSION,
    .entry_point = &demo_app_module1,
};

const FlipperApplicationLibraryDescriptor* module_demo_plugin1_ep() {
    return &demo_app_module1_descriptor;
}
