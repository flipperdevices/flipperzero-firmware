#include <gui/gui.h>
#include <dolphin/dolphin.h>
#include "../fnaf.h"
#include "cameras.h"

#define SET_DIFFICULTY(difficulties)  \
        fnaf->animatronics->AI[Bonnie] = difficulties[Bonnie];\
        fnaf->animatronics->AI[Chica] = difficulties[Chica];\
        fnaf->animatronics->AI[Freddy] = difficulties[Freddy];\
        fnaf->animatronics->AI[Foxy] = difficulties[Foxy]

void office_draw(Canvas* canvas, Fnaf* fnaf);

void office_input(Fnaf* fnaf);

void night_start(Fnaf* fnaf);

void reset_animatronic_positions(Fnaf* fnaf);

void timer_callback_bonnie(void* ctx);

void timer_callback_chica(void* ctx);

void timer_callback_freddy(void* ctx);

void timer_callback_foxy(void* ctx);

void hourly_timer_callback(void* ctx);
