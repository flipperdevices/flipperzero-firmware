#pragma once

#include <gui/icon.h>
#include <m-list.h>

typedef struct {
    const char* name;
    const char* path;
    const Icon* icon;
} MainMenuApp;

LIST_DEF(MainMenuList, MainMenuApp, M_POD_OPLIST)

typedef struct {
    const char* name;
    const char* path;
    const Icon* icon;
} GamesMenuApp;

LIST_DEF(GamesMenuList, GamesMenuApp, M_POD_OPLIST)
