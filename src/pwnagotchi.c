#include "../include/pwnagotchi.h"

#include "../include/constants.h"

#include <stdlib.h>

Pwnagotchi* pwnagotchi_alloc() {
    Pwnagotchi* pwn = malloc(sizeof(Pwnagotchi));

    pwn->channel = PWNAGOTCHI_UART_CHANNEL;
    pwn->baudRate = PWNAGOTCHI_UART_BAUD;

    furi_hal_uart_init(pwn->channel, pwn->baudRate);

    return pwn;
}

void pwnagotchi_free(Pwnagotchi* pwn) {
    furi_hal_uart_deinit(pwn->channel);

    free(pwn);
    
    pwn = NULL;

}