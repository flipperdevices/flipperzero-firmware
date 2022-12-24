#include "../include/pwnagotchi.h"

#include "../include/faces_xbm.h"

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
    strncpy(pwn->uptime, "00:00:00", 9);
    strncpy(pwn->message, "Hack the planet!", 17);
    pwn->mode = PwnMode_Manual;

    return pwn;
}

void pwnagotchi_draw_face(Pwnagotchi* pwn, Canvas* canvas) {
    UNUSED(pwn);

    canvas_draw_xbm(canvas, PWNAGOTCHI_FACE_J, PWNAGOTCHI_FACE_I, smart_width, smart_height, smart_xbm);
}

void pwnagotchi_draw_name(Pwnagotchi* pwn, Canvas* canvas) {
    char* formatName = malloc(sizeof(char) * (PWNAGOTCHI_MAX_HOSTNAME_LEN + 2));
    strncpy(formatName, pwn->hostname, PWNAGOTCHI_MAX_HOSTNAME_LEN);
    strncat(formatName, ">", 2);
    canvas_set_font(canvas, PWNAGOTCHI_FONT);
    canvas_draw_str(canvas, PWNAGOTCHI_NAME_J, PWNAGOTCHI_NAME_I, formatName);
    free(formatName);
}

void pwnagotchi_draw_channel(Pwnagotchi* pwn, Canvas* canvas) {
    char* formatChannel = malloc(sizeof(char) * (PWNAGOTCHI_MAX_CHANNEL_LEN + 4));
    strncpy(formatChannel, "CH ", 4);
    strcat(formatChannel, pwn->channel);
    canvas_set_font(canvas, PWNAGOTCHI_FONT);
    canvas_draw_str(canvas, PWNAGOTCHI_CHANNEL_J, PWNAGOTCHI_CHANNEL_I, formatChannel);
    free(formatChannel);
}

void pwnagotchi_draw_aps(Pwnagotchi* pwn, Canvas* canvas) {
    char* formatAP = malloc(sizeof(char) * (PWNAGOTCHI_MAX_APS_LEN + 5));
    strncpy(formatAP, "APS ", 5);
    strcat(formatAP, pwn->apStat);
    canvas_set_font(canvas, PWNAGOTCHI_FONT);
    canvas_draw_str(canvas, PWNAGOTCHI_APS_J, PWNAGOTCHI_APS_I, formatAP);
    free(formatAP);
}

void pwnagotchi_draw_uptime(Pwnagotchi* pwn, Canvas* canvas) {
    char* formatUp = malloc(sizeof(char) * (PWNAGOTCHI_MAX_UPTIME_LEN + 4));
    strncpy(formatUp, "UP ", 4);
    strcat(formatUp, pwn->uptime);
    canvas_set_font(canvas, PWNAGOTCHI_FONT);
    canvas_draw_str(canvas, PWNAGOTCHI_UPTIME_J, PWNAGOTCHI_UPTIME_I, formatUp);
    free(formatUp);
}

void pwnagotchi_draw_lines(Pwnagotchi* pwn, Canvas* canvas) {
    UNUSED(pwn);
    // Line 1
    canvas_draw_line(canvas, PWNAGOTCHI_LINE1_START_J, PWNAGOTCHI_LINE1_START_I, PWNAGOTCHI_LINE1_END_J, PWNAGOTCHI_LINE1_END_I);

    // Line 2
    canvas_draw_line(canvas, PWNAGOTCHI_LINE2_START_J, PWNAGOTCHI_LINE2_START_I, PWNAGOTCHI_LINE2_END_J, PWNAGOTCHI_LINE2_END_I);
}

void pwnagotchi_draw_handshakes(Pwnagotchi* pwn, Canvas* canvas) {
    char* formatShakes = malloc(sizeof(char) * (PWNAGOTCHI_MAX_HANDSHAKES_LEN + 5));
    strncpy(formatShakes, "PWND ", 6);
    strcat(formatShakes, pwn->handshakes);
    canvas_set_font(canvas, PWNAGOTCHI_FONT);
    canvas_draw_str(canvas, PWNAGOTCHI_HANDSHAKES_J, PWNAGOTCHI_HANDSHAKES_I, formatShakes);
    free(formatShakes);
}

void pwnagotchi_draw_friend(Pwnagotchi* pwn, Canvas* canvas) {
    UNUSED(pwn);
    UNUSED(canvas);
}

void pwnagotchi_draw_mode(Pwnagotchi* pwn, Canvas* canvas) {
    canvas_set_font(canvas, PWNAGOTCHI_FONT);
    switch (pwn->mode) {
        case PwnMode_Manual:
            canvas_draw_str(canvas, PWNAGOTCHI_MODE_MANU_J, PWNAGOTCHI_MODE_MANU_I, "MANU");
            break;
        case PwnMode_Auto:
            canvas_draw_str(canvas, PWNAGOTCHI_MODE_AUTO_J, PWNAGOTCHI_MODE_AUTO_I, "AUTO");
            break;
        case PwnMode_Ai:
            canvas_draw_str(canvas, PWNAGOTCHI_MODE_AI_J, PWNAGOTCHI_MODE_AI_I, "AI");
            break;
    }

}

void pwnagotchi_draw_message(Pwnagotchi* pwn, Canvas* canvas) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, PWNAGOTCHI_MESSAGE_J, PWNAGOTCHI_MESSAGE_I, pwn->message);
}

void pwnagotchi_free(Pwnagotchi* pwn) {
    free(pwn);
    pwn = NULL;

}

void pwnagotchi_screen_clear(Pwnagotchi* pwn) {
    UNUSED(pwn);
}
