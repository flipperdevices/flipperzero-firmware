#pragma once

#include "furi.h"
#include "furi_test/furi_test.h"

typedef struct {
    FlipperApplication app;
    const char* name;
} FlipperStartupApp;

const FlipperStartupApp FLIPPER_STARTUP[] = {
    {.app = furi_test_app, .name = "furi_test"}
};