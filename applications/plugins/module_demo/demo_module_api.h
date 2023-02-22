#pragma once

#define DEMO_APP_ID "demo_app"
#define DEMO_APP_MODULE_API_VERSION 1

typedef struct {
    const char* name;
    int (*method1)();
    int (*method2)(int, int);
} DemoAppModule;
