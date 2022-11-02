#include "../include/pwnagotchi.h"

#include <stdlib.h>
#include <string.h>

#include <furi/core/check.h>

Pwnagotchi* pwnagotchi_alloc() {
    Pwnagotchi* pwn = malloc(sizeof(Pwnagotchi));

    return pwn;
}

void pwnagotchi_free(Pwnagotchi* pwn) {
    free(pwn);
    pwn = NULL;

}

bool pwnagotchi_screen_set(Pwnagotchi* pwn, uint8_t i, uint8_t j, bool status) {
    if (i >= FLIPPER_SCREEN_HEIGHT || j >= FLIPPER_SCREEN_WIDTH) {
        return false;
    }

    pwn->screen[i][j] = status;
    return true;
}

void pwnagotchi_screen_clear(Pwnagotchi* pwn) {
    memset(pwn->screen, 0, sizeof(pwn->screen));
}

void pwnagotchi_screen_flush(Canvas* canvas, void* context) {
    const Pwnagotchi* pwn = context;
    canvas_clear(canvas);

    // Loop through and set each bit
    for (size_t ii = 0; ii < FLIPPER_SCREEN_HEIGHT; ii++) {
        for (size_t jj = 0; jj < FLIPPER_SCREEN_WIDTH; jj++) {
            if (pwn->screen[ii][jj]) {
                canvas_draw_dot(canvas, ii, jj);
            }
        }
    }
}