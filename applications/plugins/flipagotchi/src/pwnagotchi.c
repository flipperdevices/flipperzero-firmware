#include "../include/pwnagotchi.h"

#include "../include/faces_xbm.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <furi.h>

Pwnagotchi* pwnagotchi_alloc() {
    Pwnagotchi* pwn = malloc(sizeof(Pwnagotchi));

    pwn->face = Cool;
    pwn->friendFace = NoFace;

    strncpy(pwn->channel, "*", 2);
    strncpy(pwn->apStat, "0 (0)", 6);
    strncpy(pwn->hostname, "pwn", 4);
    strncpy(pwn->handshakes, "0 (0)", 6);
    strncpy(pwn->uptime, "00:00:00", 9);
    strncpy(pwn->message, "Hack the planet!", 17);
    pwn->mode = PwnMode_Manual;

    return pwn;
}

void pwnagotchi_draw_face(Pwnagotchi* pwn, Canvas* canvas) {
    int width = 0;
    int height = 0;
    uint8_t* xbm;
    switch(pwn->face) {
    case NoFace:
        width = noface_width;
        height = noface_height;
        xbm = noface_xbm;
        break;
    case DefaultFace:
        width = awake_width;
        height = awake_height;
        xbm = awake_xbm;
        break;
    case Look_r:
        width = look_r_width;
        height = look_r_height;
        xbm = look_r_xbm;
        break;
    case Look_l:
        width = look_l_width;
        height = look_l_height;
        xbm = look_l_xbm;
        break;
    case Look_r_happy:
        width = look_r_happy_width;
        height = look_r_happy_height;
        xbm = look_r_happy_xbm;
        break;
    case Look_l_happy:
        width = look_l_happy_width;
        height = look_l_happy_height;
        xbm = look_l_happy_xbm;
        break;
    case Sleep:
        width = sleep_width;
        height = sleep_height;
        xbm = sleep_xbm;
        break;
    case Sleep2:
        width = sleep2_width;
        height = sleep2_height;
        xbm = sleep2_xbm;
        break;
    case Awake:
        width = awake_width;
        height = awake_height;
        xbm = awake_xbm;
        break;
    case Bored:
        width = bored_width;
        height = bored_height;
        xbm = bored_xbm;
        break;
    case Intense:
        width = intense_width;
        height = intense_height;
        xbm = intense_xbm;
        break;
    case Cool:
        width = cool_width;
        height = cool_height;
        xbm = cool_xbm;
        break;
    case Happy:
        width = happy_width;
        height = happy_height;
        xbm = happy_xbm;
        break;
    case Grateful:
        width = grateful_width;
        height = grateful_height;
        xbm = grateful_xbm;
        break;
    case Excited:
        width = excited_width;
        height = excited_height;
        xbm = excited_xbm;
        break;
    case Motivated:
        width = motivated_width;
        height = motivated_height;
        xbm = motivated_xbm;
        break;
    case Demotivated:
        width = demotivated_width;
        height = demotivated_height;
        xbm = demotivated_xbm;
        break;
    case Smart:
        width = smart_width;
        height = smart_height;
        xbm = smart_xbm;
        break;
    case Lonely:
        width = lonely_width;
        height = lonely_height;
        xbm = lonely_xbm;
        break;
    case Sad:
        width = sad_width;
        height = sad_height;
        xbm = sad_xbm;
        break;
    case Angry:
        width = angry_width;
        height = angry_height;
        xbm = angry_xbm;
        break;
    case Friend:
        width = friend_width;
        height = friend_height;
        xbm = friend_xbm;
        break;
    case Broken:
        width = broken_width;
        height = broken_height;
        xbm = broken_xbm;
        break;
    case Debug:
        width = debug_width;
        height = debug_height;
        xbm = debug_xbm;
        break;
    case Upload:
        width = upload_width;
        height = upload_height;
        xbm = upload_xbm;
        break;
    case Upload1:
        width = upload1_width;
        height = upload1_height;
        xbm = upload1_xbm;
        break;
    case Upload2:
        width = upload2_width;
        height = upload2_height;
        xbm = upload2_xbm;
        break;
    default:
        xbm = NULL;
    }

    canvas_draw_xbm(canvas, PWNAGOTCHI_FACE_J, PWNAGOTCHI_FACE_I, width, height, xbm);
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
    strncpy(formatChannel, "CH", 3);
    strcat(formatChannel, pwn->channel);
    canvas_set_font(canvas, PWNAGOTCHI_FONT);
    canvas_draw_str(canvas, PWNAGOTCHI_CHANNEL_J, PWNAGOTCHI_CHANNEL_I, formatChannel);
    free(formatChannel);
}

void pwnagotchi_draw_aps(Pwnagotchi* pwn, Canvas* canvas) {
    char* formatAP = malloc(sizeof(char) * (PWNAGOTCHI_MAX_APS_LEN + 5));
    strncpy(formatAP, "APS", 4);
    strcat(formatAP, pwn->apStat);
    canvas_set_font(canvas, PWNAGOTCHI_FONT);
    canvas_draw_str(canvas, PWNAGOTCHI_APS_J, PWNAGOTCHI_APS_I, formatAP);
    free(formatAP);
}

void pwnagotchi_draw_uptime(Pwnagotchi* pwn, Canvas* canvas) {
    char* formatUp = malloc(sizeof(char) * (PWNAGOTCHI_MAX_UPTIME_LEN + 4));
    strncpy(formatUp, "UP", 3);
    strcat(formatUp, pwn->uptime);
    canvas_set_font(canvas, PWNAGOTCHI_FONT);
    canvas_draw_str(canvas, PWNAGOTCHI_UPTIME_J, PWNAGOTCHI_UPTIME_I, formatUp);
    free(formatUp);
}

void pwnagotchi_draw_lines(Pwnagotchi* pwn, Canvas* canvas) {
    UNUSED(pwn);
    // Line 1
    canvas_draw_line(
        canvas,
        PWNAGOTCHI_LINE1_START_J,
        PWNAGOTCHI_LINE1_START_I,
        PWNAGOTCHI_LINE1_END_J,
        PWNAGOTCHI_LINE1_END_I);

    // Line 2
    canvas_draw_line(
        canvas,
        PWNAGOTCHI_LINE2_START_J,
        PWNAGOTCHI_LINE2_START_I,
        PWNAGOTCHI_LINE2_END_J,
        PWNAGOTCHI_LINE2_END_I);
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
    switch(pwn->mode) {
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
    int fontHeight = canvas_current_font_height(canvas);

    // Apparently W is the widest character (USING a for a more average approach)
    size_t charLength = canvas_string_width(canvas, "a");

    size_t horizSpace = FLIPPER_SCREEN_WIDTH - PWNAGOTCHI_MESSAGE_J;
    size_t charSpaces = floor(((double)horizSpace) / charLength);
    size_t messagePixLen = canvas_string_width(canvas, pwn->message);
    size_t maxLines =
        floor((PWNAGOTCHI_MESSAGE_I - PWNAGOTCHI_LINE2_END_I) / ((double)fontHeight));

    size_t requiredLines = ceil(((double)messagePixLen) / horizSpace);

    size_t charIndex = 0;
    for(size_t i = 0; i < requiredLines && i < maxLines - 1; i++) {
        // Allocate the line with room for two more characters (a space and then another char)
        size_t allocSize = charSpaces + 2;
        char* line = malloc(sizeof(char) * allocSize);

        // Copy the allotted characters into line
        memcpy(line, (pwn->message + charIndex), allocSize);

        // Now loop backwards and cut it off at a space if we end with a letter
        size_t backspaceCount = 0;
        if(line[allocSize - 1] != ' ' && line[allocSize - 1] != '\0') {
            for(int j = allocSize - 1; j >= 0; j--) {
                if(line[j] == ' ') {
                    line[j] = '\0';
                    break;
                }
                backspaceCount++;
            }
        }

        // Lets make sure if backspaceCount is too large that we cut the word instead of drawing off the screen
        if(backspaceCount >= charSpaces) {
            backspaceCount = 0;
        }

        canvas_draw_str(
            canvas, PWNAGOTCHI_MESSAGE_J, PWNAGOTCHI_MESSAGE_I + (i * fontHeight), line);

        charIndex += (charSpaces - backspaceCount + 1);
        free(line);
    }
}

void pwnagotchi_draw_all(Pwnagotchi* pwn, Canvas* canvas) {
    pwnagotchi_draw_face(pwn, canvas);
    pwnagotchi_draw_name(pwn, canvas);
    pwnagotchi_draw_channel(pwn, canvas);
    pwnagotchi_draw_aps(pwn, canvas);
    pwnagotchi_draw_uptime(pwn, canvas);
    pwnagotchi_draw_lines(pwn, canvas);
    pwnagotchi_draw_friend(pwn, canvas);
    pwnagotchi_draw_mode(pwn, canvas);
    pwnagotchi_draw_handshakes(pwn, canvas);
    pwnagotchi_draw_message(pwn, canvas);
}

void pwnagotchi_free(Pwnagotchi* pwn) {
    free(pwn);
    pwn = NULL;
}

void pwnagotchi_screen_clear(Pwnagotchi* pwn) {
    UNUSED(pwn);
}
