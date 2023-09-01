#ifndef SELECCT_MOVE2_HPP
#define SELECCT_MOVE2_HPP

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
} SelectMove2;

SelectMove2* select_move2_alloc(App* app);

void select_move2_free(App* app);

View* select_move2_get_view(App* app);

#endif /* SELECCT_MOVE2_HPP */