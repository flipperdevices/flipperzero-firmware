#include "../include/pwnagotchi.h"

#include <stdlib.h>
#include <string.h>

#include <furi.h>

Pwnagotchi* pwnagotchi_alloc() {
    Pwnagotchi* pwn = malloc(sizeof(Pwnagotchi));

    pwn->face = Grateful;
    pwn->faceStr = GRATEFUL;
    pwn->friendFace = NoFace;

    strncpy(pwn->channel, "*", 2);
    strncpy(pwn->apStat, "0 (0)", 6);
    strncpy(pwn->hostname, "pwn", 4);
    strncpy(pwn->handshakes, "0 (0)", 6);
    strncpy(pwn->message, "Hello World!", 13);
    pwn->mode = Ai;

    return pwn;
}

void pwnagotchi_draw_face(Pwnagotchi* pwn, Canvas* canvas) {
    canvas_set_font(canvas, FontKeyboard);

    canvas_draw_str(canvas, PWNAGOTCHI_FACE_J, PWNAGOTCHI_FACE_I, pwn->faceStr);
}

void pwnagotchi_draw_name(Pwnagotchi* pwn, Canvas* canvas) {
    char* formatName = malloc(sizeof(char) * (PWNAGOTCHI_MAX_HOSTNAME_LEN + 1));
    strncpy(formatName, pwn->hostname, PWNAGOTCHI_MAX_HOSTNAME_LEN);
    strncat(formatName, ">", 2);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, PWNAGOTCHI_NAME_J, PWNAGOTCHI_NAME_I, formatName);
    free(formatName);
}

void pwnagotchi_draw_channel(Pwnagotchi* pwn, Canvas* canvas) {
    UNUSED(pwn);
    char* formatChannel = malloc(sizeof(char) * (PWNAGOTCHI_MAX_CHANNEL_LEN + 3));
    strncpy(formatChannel, "CH ", 4);
    // strncat(formatChannel, pwn->channel, PWNAGOTCHI_MAX_CHANNEL_LEN);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, PWNAGOTCHI_CHANNEL_J, PWNAGOTCHI_CHANNEL_I, formatChannel);
    free(formatChannel);
}

void pwnagotchi_draw_aps(Pwnagotchi* pwn, Canvas* canvas) {
    UNUSED(pwn);
    UNUSED(canvas);
}

void pwnagotchi_draw_uptime(Pwnagotchi* pwn, Canvas* canvas) {
    UNUSED(pwn);
    UNUSED(canvas);
}

void pwnagotchi_draw_lines(Pwnagotchi* pwn, Canvas* canvas) {
    UNUSED(pwn);
    // Line 1
    canvas_draw_line(canvas, PWNAGOTCHI_LINE1_START_J, PWNAGOTCHI_LINE1_START_I, PWNAGOTCHI_LINE1_END_J, PWNAGOTCHI_LINE1_END_I);

    // Line 2
    canvas_draw_line(canvas, PWNAGOTCHI_LINE2_START_J, PWNAGOTCHI_LINE2_START_I, PWNAGOTCHI_LINE2_END_J, PWNAGOTCHI_LINE2_END_I);
}

void pwnagotchi_draw_friend(Pwnagotchi* pwn, Canvas* canvas) {
    UNUSED(pwn);
    UNUSED(canvas);
}

void pwnagotchi_draw_mode(Pwnagotchi* pwn, Canvas* canvas) {
    UNUSED(pwn);
    UNUSED(canvas);
}

void pwnagotchi_free(Pwnagotchi* pwn) {
    free(pwn);
    pwn = NULL;

}

void pwnagotchi_screen_clear(Pwnagotchi* pwn) {
    UNUSED(pwn);;
}
