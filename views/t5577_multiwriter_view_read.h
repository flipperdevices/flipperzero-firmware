#pragma once
#include <gui/view.h>

typedef enum {
    LfRfidReadAsk,
    LfRfidReadPsk,
    LfRfidReadAskOnly,
    LfRfidReadPskOnly
} LfRfidReadViewMode;

typedef struct LfRfidReadView LfRfidReadView;

LfRfidReadView* t5577_multiwriter_view_read_alloc();

void t5577_multiwriter_view_read_free(LfRfidReadView* read_view);

View* t5577_multiwriter_view_read_get_view(LfRfidReadView* read_view);

void t5577_multiwriter_view_read_set_read_mode(LfRfidReadView* read_view, LfRfidReadViewMode mode);
