#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>

View* genie_learn_alloc(void* app);

void genie_learn_free(View* view);