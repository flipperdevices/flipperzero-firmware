#pragma once

#include <gui/view.h>

typedef struct ResetView ResetView;

ResetView* reset_view_alloc();

void reset_view_free(ResetView* reset_view);

View* reset_view_get_view(ResetView* reset_view);