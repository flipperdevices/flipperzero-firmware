#include "main_callbacks.h"

void main_draw_callback(Canvas *canvas, void *ctx)
{
    UNUSED(canvas);
    UNUSED(ctx);
}

void main_input_callback(InputEvent *event, void *ctx)
{
    UNUSED(event);
    UNUSED(ctx);
}

int32_t main_thread_callback(void *ctx)
{
    UNUSED(ctx);
    return 0;
}
