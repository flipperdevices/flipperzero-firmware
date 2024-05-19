#include "eth_worker.h"
#include <furi_hal.h>
#include <ping.h>

void ping_wait_ms(int ms) {
    furi_delay_ms(ms);
}

uint8_t ping_auto_interface(uint8_t* adress) {
    return ping_auto(PING_SOCKET, adress);
}