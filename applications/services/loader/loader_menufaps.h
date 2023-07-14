#pragma once

#include <gui/icon.h>
#include <m-list.h>

typedef struct {
    const char* name;
    const char* path;
    const Icon* icon;
} MenuFap;

LIST_DEF(MenuFapsList, MenuFap, M_POD_OPLIST)
