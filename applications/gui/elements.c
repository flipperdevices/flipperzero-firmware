#include "elements.h"

void elements_scrollbar(CanvasApi* canvas, uint8_t pos, uint8_t total) {
    uint8_t width = canvas->width(canvas);
    uint8_t height = canvas->height(canvas);

    for (uint8_t i=1; i<height; i+=2) {
        canvas->draw_dot(canvas, width-2, i);
    }

    uint8_t block_h = ((float)height) / total;

    canvas->draw_box(canvas, width-3, block_h*pos, 3, block_h);
}