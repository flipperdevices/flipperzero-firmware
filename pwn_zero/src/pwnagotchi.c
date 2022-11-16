#include "../include/pwnagotchi.h"

#include <stdlib.h>
#include <string.h>

#include <furi.h>

Pwnagotchi* pwnagotchi_alloc() {
    Pwnagotchi* pwn = malloc(sizeof(Pwnagotchi));

    pwn->face = Grateful;
    pwn->faceStr = GRATEFUL;
    pwn->friendFace = NoFace;

    return pwn;
}

// void pwnagotchi_draw_face(Pwnagotchi* pwn, Canvas* canvas) {
//     canvas_set_font(canvas, FontKeyboard);

//     canvas_draw_str(canvas, PWNAGOTCHI_FACE_J, PWNAGOTCHI_FACE_I, pwn->faceStr);
// }

void pwnagotchi_free(Pwnagotchi* pwn) {
    free(pwn);
    pwn = NULL;

}

void pwnagotchi_screen_clear(Pwnagotchi* pwn) {
    UNUSED(pwn);
}
