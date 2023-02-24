#pragma once

#define APP_ID "module_demo_advanced"
#define APP_MODULE_API_VERSION 1

typedef struct {
    const char* name;
    void (*method1)(int);
    void (*method2)();
} AdvancedDemoAppModule;
