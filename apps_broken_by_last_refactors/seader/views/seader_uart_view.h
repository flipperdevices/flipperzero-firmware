#pragma once

#include <gui/view.h>
#include "../seader_custom_event.h"
#include "../seader_bridge.h"

typedef struct SeaderUartView SeaderUartView;
typedef void (*SeaderUartViewCallback)(SeaderCustomEvent event, void* context);

SeaderUartView* seader_uart_view_alloc();

void seader_uart_view_free(SeaderUartView* seader_uart_view);

View* seader_uart_view_get_view(SeaderUartView* seader_uart_view);

void seader_uart_view_set_callback(
    SeaderUartView* seader_uart_view,
    SeaderUartViewCallback callback,
    void* context);

void seader_uart_view_update_state(
    SeaderUartView* instance,
    SeaderUartConfig* cfg,
    SeaderUartState* st);
