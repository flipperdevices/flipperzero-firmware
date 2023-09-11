#ifndef SELECCT_MOVE3_HPP
#define SELECCT_MOVE3_HPP

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
} SelectMove3;

SelectMove3* select_move3_alloc(App* app);

void select_move3_free(App* app);

View* select_move3_get_view(App* app);

#endif /* SELECCT_MOVE3_HPP */