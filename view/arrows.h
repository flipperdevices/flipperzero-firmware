#pragma once
#include <gui/canvas.h>
// Function declarations

/*
    * Swaps the values of two integers.
    *
    * @param xp The first integer.
    * @param yp The second integer.
*/
void swap(int* xp, int* yp);

/**
 * Draws an arrow on the canvas, which can be either filled or empty.
 *
 * @param canvas The canvas to draw on.
 * @param pos_x The x position to start drawing from.
 * @param pos_y The y position to start drawing from.
 * @param direction The direction of the arrow ('U', 'D', 'L', 'R').
 * @param fill Whether the arrow should be filled (true) or empty (false).
 */
void draw_empty_or_filled_arrow(
    Canvas* const canvas,
    int8_t pos_x,
    int8_t pos_y,
    char direction,
    bool fill);

/**
 * Draws a filled triangle on the canvas.
 *
 * @param canvas The canvas to draw on.
 * @param x1 The x coordinate of the first vertex.
 * @param y1 The y coordinate of the first vertex.
 * @param x2 The x coordinate of the second vertex.
 * @param y2 The y coordinate of the second vertex.
 * @param x3 The x coordinate of the third vertex.
 * @param y3 The y coordinate of the third vertex.
 */
void canvas_draw_filled_triangle(
    Canvas* const canvas,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    int32_t x3,
    int32_t y3);
