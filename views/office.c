#include "office.h"

void office_draw() {

}

void night_start(Fnaf* fnaf) {

    if (fnaf->progress > 6) {
        fnaf->progress = 0;
        fnaf->current_view = main_menu;
    }
    reset_animatronic_positions(fnaf);
    fnaf->power_left = 100;
    // What else?

}

void reset_animatronic_positions(Fnaf* fnaf) {
    fnaf->animatronics->location[Bonnie] = cam1A;
    fnaf->animatronics->location[Chica] = cam1A;
    fnaf->animatronics->location[Freddy] = cam1A;
    fnaf->animatronics->location[Foxy] = cam1C;
}

void timer_callback_bonnie(void* ctx) {
    UNUSED(ctx);
}

void timer_callback_chica(void* ctx) {
    UNUSED(ctx);
}

void timer_callback_freddy(void* ctx) {
    UNUSED(ctx);
}

void timer_callback_foxy(void* ctx) {
    UNUSED(ctx);
}
