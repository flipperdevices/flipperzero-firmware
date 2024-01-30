#pragma once
#include <stdbool.h>
#include <storage/storage.h>
#include <gui/canvas.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Sprite Sprite;

/** Sprite allocator
 * @return Sprite*  Sprite instance
 */
Sprite* sprite_alloc();

/** Load sprite from bmp file
 * @param sprite Sprite instance
 * @param storage Storage instance
 * @param bmp_path path to bmp file
 * @return bool true if success
 */
bool sprite_load_from_bmp(Sprite* sprite, Storage* storage, const char* bmp_path);

/** Sprite deallocator
 * @param sprite Sprite instance
 */
void sprite_free(Sprite* sprite);

/** Get sprite width
 * @param sprite Sprite instance
 * @return size_t sprite width
 */
size_t sprite_get_width(Sprite* sprite);

/** Get sprite height
 * @param sprite Sprite instance
 * @return size_t sprite height
 */
size_t sprite_get_height(Sprite* sprite);

/** Draw sprite on canvas
 * @param canvas Canvas instance
 * @param sprite Sprite instance
 * @param x x coordinate
 * @param y y coordinate
 */
void canvas_draw_sprite(Canvas* canvas, Sprite* sprite, int32_t x, int32_t y);

#ifdef __cplusplus
}
#endif