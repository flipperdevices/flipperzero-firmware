#ifndef __GUI_H__
#define __GUI_H__

#include <gui/gui.h>

#include "flipper_structs.h"
#include "game_structs.h"

void refresh_gui(ViewPort *);

/* Understand which image to display and draw it */
void decode_and_draw_image(Canvas *, struct GameState *);

#endif
