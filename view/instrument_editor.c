#include "instrument_editor.h"

void draw_instrument_view(Canvas *canvas, FlizzerTrackerApp *tracker)
{
    canvas_draw_line(canvas, 0, 0, 10, 10);
    UNUSED(canvas);
    UNUSED(tracker);
}

void draw_instrument_program_view(Canvas *canvas, FlizzerTrackerApp *tracker)
{
    UNUSED(canvas);
    UNUSED(tracker);
}