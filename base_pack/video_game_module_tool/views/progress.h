/**
 * @file progress.h
 * @brief Gui view used to display the flashing progress.
 *
 * Includes a progress bar and some static graphics.
 */
#pragma once

#include <gui/view.h>

typedef struct Progress Progress;

Progress* progress_alloc();

void progress_free(Progress* instance);

View* progress_get_view(Progress* instance);

void progress_set_value(Progress* instance, uint8_t value);

void progress_reset(Progress* instance);
