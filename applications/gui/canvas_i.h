#pragma once

canvas_t * canvas_alloc();

void canvas_free(canvas_t *canvas);

void canvas_commit(canvas_t *canvas);

void canvas_frame_set(canvas_t *canvas, uint8_t offset_x, uint8_t offset_y, uint8_t width, uint8_t height);
