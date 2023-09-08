#ifndef SELECCT_MOVE1_HPP
#define SELECCT_MOVE1_HPP

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
} SelectMove1;

SelectMove1* select_move1_alloc(App* app);

void select_move1_free(App* app);

View* select_move1_get_view(App* app);

#endif /* SELECCT_MOVE1_HPP */