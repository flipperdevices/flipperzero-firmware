#include "sprite.h"

struct Sprite {
    uint8_t width;
    uint8_t height;
    uint8_t width_in_bytes;
    uint8_t* data;
};

#define TAG "Sprite"

static void sprite_set_pixel(Sprite* sprite, uint8_t x, uint8_t y, bool color) {
    size_t p = y * sprite->width_in_bytes + x / 8;
    size_t b = x % 8;

    if(color) {
        sprite->data[p] |= 1 << b;
    } else {
        sprite->data[p] &= ~(1 << b);
    }
}

static bool sprite_load(Storage* storage, const char* bmp_path, Sprite* sprite) {
    bool result = false;
    File* file = storage_file_alloc(storage);

    do {
        if(!storage_file_open(file, bmp_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_E(TAG, "Failed to open file: %s", bmp_path);
            break;
        }

        // actually header is 54 bytes, but we only need first 38
        uint8_t header[38];

        if(!storage_file_read(file, header, sizeof(header))) {
            FURI_LOG_E(TAG, "Failed to read file header");
            break;
        }

        // check if it's a bmp file
        if(header[0] != 'B' || header[1] != 'M') {
            FURI_LOG_E(TAG, "Invalid BMP header");
            break;
        }

        // load bmp info
        const uint32_t data_offset = *(uint32_t*)&header[10];
        const uint32_t width = *(uint32_t*)&header[18];
        const uint32_t height = *(uint32_t*)&header[22];
        const uint16_t bpp = *(uint16_t*)&header[28];
        const uint32_t size = *(uint32_t*)&header[34];

        uint8_t* data = malloc(size);
        storage_file_seek(file, data_offset, true);
        if(!storage_file_read(file, data, size)) {
            FURI_LOG_E(TAG, "Failed to read file data");
            free(data);
            break;
        }

        sprite->width = width;
        sprite->height = height;
        sprite->width_in_bytes = width / 8 + (width % 8 ? 1 : 0);

        sprite->data = malloc(sprite->width_in_bytes * sprite->height);

        // convert bmp data to 1-bit xbm
        size_t p = 0;
        size_t x = 0;
        size_t y = 0;
        const size_t bpp_in_bytes = bpp / 8;

        while(p < size) {
            // sum all bytes in pixel
            uint32_t bmp_px = 0;
            for(size_t i = 0; i < bpp_in_bytes; i++) {
                bmp_px += data[p + i];
            }
            p += bpp_in_bytes;

            // if sum divided by bytes per pixel is less than 128, it's a black pixel
            bool color = bmp_px / (bpp_in_bytes) < 128;

            // set pixel
            sprite_set_pixel(sprite, x, height - y - 1, color);

            // advance to next pixel
            x++;

            // advance to next row
            if(x >= width) {
                // consider that bmp data is padded to 4 bytes
                size_t row_reminder = p % 4;
                if(row_reminder) {
                    p += 4 - row_reminder;
                }

                x = 0;
                y++;
            }
        }

        free(data);

        result = true;
    } while(false);

    storage_file_free(file);
    return result;
}

Sprite* sprite_alloc() {
    Sprite* sprite = malloc(sizeof(Sprite));
    sprite->width = 0;
    sprite->height = 0;
    sprite->data = NULL;
    return sprite;
}

bool sprite_load_from_bmp(Sprite* sprite, Storage* storage, const char* bmp_path) {
    return sprite_load(storage, bmp_path, sprite);
}

void sprite_free(Sprite* sprite) {
    free(sprite->data);
    free(sprite);
}

size_t sprite_get_width(Sprite* sprite) {
    return sprite->width;
}

size_t sprite_get_height(Sprite* sprite) {
    return sprite->height;
}

void canvas_draw_sprite(Canvas* canvas, Sprite* sprite, int32_t x, int32_t y) {
    furi_check(sprite->width);
    furi_check(sprite->height);
    furi_check(sprite->data);
    canvas_draw_xbm(canvas, x, y, sprite->width, sprite->height, sprite->data);
}