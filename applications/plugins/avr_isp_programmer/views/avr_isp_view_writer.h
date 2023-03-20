#pragma once

#include <gui/view.h>
#include "../helpers/avr_isp_types.h"
#include "../helpers/avr_isp_event.h"

typedef struct AvrIspWriterView AvrIspWriterView;

typedef void (*AvrIspWriterViewCallback)(AvrIspCustomEvent event, void* context);

void avr_asp_writer_view_set_callback(
    AvrIspWriterView* instance,
    AvrIspWriterViewCallback callback,
    void* context);

AvrIspWriterView* avr_asp_writer_view_alloc();

void avr_asp_writer_view_free(AvrIspWriterView* instance);

View* avr_asp_writer_view_get_view(AvrIspWriterView* instance);

void avr_asp_writer_view_exit(void* context);
