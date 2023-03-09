#pragma once

#include <gui/view.h>
#include "../helpers/avr_isp_types.h"
#include "../helpers/avr_isp_event.h"

typedef struct AvrIspProgrammerView AvrIspProgrammerView;

typedef void (*AvrIspProgrammerViewCallback)(AvrIspCustomEvent event, void* context);

// void avr_asp_programmer_view_set_rssi(AvrIspProgrammerView* instance, float rssi);

// void avr_asp_programmer_view_set_lock(AvrIspProgrammerView* avr_asp_programmer_view, avr_asp_programmerLock keyboard);

void avr_asp_programmer_view_set_callback(
    AvrIspProgrammerView* instance,
    AvrIspProgrammerViewCallback callback,
    void* context);

AvrIspProgrammerView* avr_asp_programmer_view_alloc();

void avr_asp_programmer_view_free(AvrIspProgrammerView* instance);

View* avr_asp_programmer_view_get_view(AvrIspProgrammerView* instance);

void avr_asp_programmer_view_exit(void* context);
