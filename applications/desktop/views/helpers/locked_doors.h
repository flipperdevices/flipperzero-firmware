#pragma once

#include <gui/canvas.h>
#include <gui/view.h>

typedef struct DesktopViewsHelpersLockedDoors DesktopViewsHelpersLockedDoors;

typedef void (*DesktopViewsHelpersLockedDoorsCallback)(void*);

void desktop_views_helpers_locked_doors_draw(Canvas* canvas, DesktopViewsHelpersLockedDoors* instance);
DesktopViewsHelpersLockedDoors* desktop_views_helpers_locked_doors_alloc(void);
void desktop_views_helpers_locked_doors_free(DesktopViewsHelpersLockedDoors* instance);
void desktop_views_helpers_locked_doors_restart(DesktopViewsHelpersLockedDoors* instance, View* view);


void desktop_views_helpers_locked_doors_reset(DesktopViewsHelpersLockedDoors* instance);
bool desktop_views_helpers_locked_doors_move(DesktopViewsHelpersLockedDoors* instance);

