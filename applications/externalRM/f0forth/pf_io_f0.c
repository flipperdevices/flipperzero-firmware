#include "f0forth_cli.h"

#include "lib/pforth/pforth.h"

int sdTerminalOut(char c) {
    if(c == '\n') {
        char cr = '\r';
        cli_write(f0f_cli_state->cli, (uint8_t*)&cr, 1);
    }

    cli_write(f0f_cli_state->cli, (uint8_t*)&c, 1);
    return c;
}

int sdTerminalEcho(char c) {
    if(c == '\n') {
        char cr = '\r';
        cli_write(f0f_cli_state->cli, (uint8_t*)&cr, 1);
    }

    cli_write(f0f_cli_state->cli, (uint8_t*)&c, 1);
    return 0;
}

int sdTerminalIn(void) {
    return cli_getc(f0f_cli_state->cli);
}

int sdTerminalFlush(void) {
    return -1;
}

int sdQueryTerminal(void) {
    return 0;
}

void sdTerminalInit(void) {
}

void sdTerminalTerm(void) {
}

cell_t sdSleepMillis(cell_t msec) {
    furi_delay_ms(msec);
    return 0;
}
