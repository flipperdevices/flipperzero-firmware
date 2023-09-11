#ifndef SELECCT_STATS_HPP
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
} SelectStats;

SelectStats* select_stats_alloc(App* app);

void select_stats_free(App* app);

View* select_stats_get_view(App* app);

#endif /* SELECCT_STATS_HPP */