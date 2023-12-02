#include "icon_drawing.h"

void draw_icon_aligned(Canvas* canvas, uint8_t x, uint8_t y, Align alignHorizontal, Align alignVertical, const Icon* icon)
{
    uint8_t iconWidth = icon_get_width(icon);
    uint8_t iconHeight = icon_get_height(icon);

    uint8_t iconX = x;
    uint8_t iconY = y;

    if (alignHorizontal == AlignCenter)
        iconX = x - iconWidth / 2;
    else if (alignHorizontal == AlignRight)
        iconX = x - iconWidth;

    if (alignVertical == AlignCenter)
        iconY = y - iconHeight / 2;
    else if (alignVertical == AlignBottom)
        iconY = y - iconHeight;

    canvas_draw_icon(canvas, iconX, iconY, icon);
}