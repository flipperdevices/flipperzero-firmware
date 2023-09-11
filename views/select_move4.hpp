#ifndef SELECCT_MOVE4_HPP
#define SELECCT_MOVE4_HPP

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
} SelectMove4;

SelectMove4* select_move4_alloc(App* app);

void select_move4_free(App* app);

View* select_move4_get_view(App* app);

#endif /* SELECCT_MOVE4_HPP */