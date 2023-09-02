#include "eth_worker.h"
#include <furi_hal.h>
#include <socket.h>
#include <dhcp.h>
#include <ping.h>

uint8_t ping_count(uint8_t s, uint16_t pCount, uint8_t* addr);

void ping_wait_ms(int ms) {
    furi_delay_ms(ms);
}

uint8_t ping_auto_interface(uint8_t* adress) {
    return ping_auto(PING_SOCKET, adress);
    //    return ping_count(PING_SOCKET, 20, adress);
}