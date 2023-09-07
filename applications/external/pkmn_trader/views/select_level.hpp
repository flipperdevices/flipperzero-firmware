#ifndef SELECCT_LEVEL_HPP
#define SELECCT_LEVEL_HPP

#pragma once
#include <furi.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>

#include <gui/elements.h>
#include <string>

typedef struct App App;

typedef struct {
    View* view;
    App* app;
} SelectLevel;

SelectLevel* select_level_alloc(App* app);

void select_level_free(App* app);

View* select_level_get_view(App* app);

#endif /* SELECCT_LEVEL_HPP */