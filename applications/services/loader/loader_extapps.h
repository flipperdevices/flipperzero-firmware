#pragma once

#include <gui/icon.h>
#include <m-list.h>

typedef struct {
    const char* name;
    const char* path;
    const Icon* icon;
} ExtMainApp;

LIST_DEF(ExtMainAppList, ExtMainApp, M_POD_OPLIST)
