#include "canvas.h"
#include "canvas_i.h"

#include <assert.h>
#include <furi.h>
#include <u8g2.h>

struct canvas_t {
    FuriRecordSubscriber *fb_record;
    u8g2_t *fb;
    uint8_t offset_x;
    uint8_t offset_y;
    uint8_t width;
    uint8_t height;
};

canvas_t * canvas_alloc()
{
    canvas_t *canvas = furi_alloc(sizeof(canvas_t));
    canvas->fb_record = furi_open("u8g2_fb", false, false, NULL, NULL, NULL);
    assert(canvas->fb_record);
    return canvas;
}

void canvas_free(canvas_t *canvas)
{
    assert(canvas);
    free(canvas);
}

void canvas_commit(canvas_t *canvas)
{
    assert(canvas);
    if (canvas->fb) {
        furi_commit(canvas->fb_record);
        canvas->fb = NULL;
    }
}

void canvas_frame_set(canvas_t *canvas, uint8_t offset_x, uint8_t offset_y, uint8_t width, uint8_t height)
{
    assert(canvas);
    canvas->offset_x = offset_x;
    canvas->offset_y = offset_y;
    canvas->width = width;
    canvas->height = height;
}

u8g2_t * canvas_fb(canvas_t *canvas) {
    if (!canvas->fb) {
        canvas->fb = furi_take(canvas->fb_record);
        assert(canvas->fb);
    }
    return canvas->fb;
}

void canvas_clear(canvas_t *canvas)
{
    u8g2_t *fb = canvas_fb(canvas);
    u8g2_ClearBuffer(fb);
}

void canvas_color_set(canvas_t *canvas, uint8_t color)
{
    u8g2_t *fb = canvas_fb(canvas);
    u8g2_SetDrawColor(fb, 1);
}

void canvas_font_set(canvas_t *canvas, font_t font)
{
    u8g2_t *fb = canvas_fb(canvas);
    u8g2_SetFontMode(fb, 1);
    u8g2_SetFont(fb, font);
}

void canvas_str_draw(canvas_t *canvas, uint8_t x, uint8_t y, const char *str)
{
    x += canvas->offset_x;
    y += canvas->offset_y;
    u8g2_t *fb = canvas_fb(canvas);
    u8g2_DrawStr(fb, x, y, str);
}
