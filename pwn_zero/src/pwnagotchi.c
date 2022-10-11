#include "../include/pwnagotchi.h"

#include <stdlib.h>
#include <string.h>

Pwnagotchi* pwnagotchi_alloc() {
    Pwnagotchi* pwn = malloc(sizeof(Pwnagotchi));

    pwn->channel = PWNAGOTCHI_UART_CHANNEL;
    pwn->baudRate = PWNAGOTCHI_UART_BAUD;

    furi_hal_uart_init(pwn->channel, pwn->baudRate);

    // Turn off all pixels in screen array
    memset(pwn->screen, 0, sizeof(pwn->screen));

    return pwn;
}

void pwnagotchi_free(Pwnagotchi* pwn) {
    furi_hal_uart_deinit(pwn->channel);

    free(pwn);
    
    pwn = NULL;

}