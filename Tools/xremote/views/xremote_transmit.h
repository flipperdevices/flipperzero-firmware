#pragma once

#include <gui/view.h>
#include "../helpers/xremote_custom_event.h"

typedef struct XRemoteTransmit XRemoteTransmit;

typedef void (*XRemoteTransmitCallback)(XRemoteCustomEvent event, void* context);

void xremote_transmit_model_set_name(XRemoteTransmit* instance, const char* name);
void xremote_transmit_model_set_type(XRemoteTransmit* instance, int type);

void xremote_transmit_set_callback(
    XRemoteTransmit* instance,
    XRemoteTransmitCallback callback,
    void* context);

XRemoteTransmit* xremote_transmit_alloc();

void xremote_transmit_free(XRemoteTransmit* instance);

View* xremote_transmit_get_view(XRemoteTransmit* instance);

void xremote_transmit_enter(void* context);