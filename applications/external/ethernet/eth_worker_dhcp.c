#include "eth_worker.h"
#include <furi_hal.h>
#include <dhcp.h>

void dhcp_timer_callback(void* context) {
    UNUSED(context);
    DHCP_time_handler();
}