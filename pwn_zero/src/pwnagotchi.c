#include "../include/pwnagotchi.h"

#include <stdlib.h>
#include <string.h>

#include <furi/core/check.h>

Pwnagotchi* pwnagotchi_alloc() {
    Pwnagotchi* pwn = malloc(sizeof(Pwnagotchi));

    pwn->channel = PWNAGOTCHI_UART_CHANNEL;
    pwn->baudRate = PWNAGOTCHI_UART_BAUD;

    furi_hal_uart_init(pwn->channel, pwn->baudRate);

    // Turn off all pixels in screen array
    memset(pwn->screen, 0, sizeof(pwn->screen));

    // Allocate message buffer
    pwn->messageQueue = malloc(sizeof(uint8_t) * PWNAGOTCHI_PROTOCOL_BYTE_LEN * PWNAGOTCHI_PROTOCOL_QUEUE_SIZE);
    pwn->queueSize = PWNAGOTCHI_PROTOCOL_BYTE_LEN * PWNAGOTCHI_PROTOCOL_QUEUE_SIZE;

    pwn->readIdx = pwn->messageQueue;
    pwn->writeIdx = pwn->messageQueue;

    return pwn;
}

void pwnagotchi_free(Pwnagotchi* pwn) {
    furi_hal_uart_deinit(pwn->channel);

    free(pwn);
    
    pwn = NULL;

}
