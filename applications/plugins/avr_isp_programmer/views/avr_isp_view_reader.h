#pragma once

#include <gui/view.h>
#include "../helpers/avr_isp_types.h"
#include "../helpers/avr_isp_event.h"

typedef struct AvrIspReaderView AvrIspReaderView;

typedef void (*AvrIspReaderViewCallback)(AvrIspCustomEvent event, void* context);

void avr_asp_reader_view_set_callback(
    AvrIspReaderView* instance,
    AvrIspReaderViewCallback callback,
    void* context);

AvrIspReaderView* avr_asp_reader_view_alloc();

void avr_asp_reader_view_free(AvrIspReaderView* instance);

View* avr_asp_reader_view_get_view(AvrIspReaderView* instance);

void avr_asp_reader_view_exit(void* context);
