#pragma once

#include <stdint.h>

struct FontCharInfo_s {
    /**
     * @brief Width of the character
     */
    uint8_t width;

    /**
     * @brief Offset of the character's bitmap, in bytes, into the the data array
     */
    uint16_t offset;
} __attribute__((packed));

typedef struct FontCharInfo_s FontCharInfo;

typedef struct {
    /**
     * @brief Font name
     */
    char* name;

    /**
     * @brief Font characters height
     */
    uint8_t height;

    /**
     * @brief The first character available in the font
     */
    uint8_t start_char;

    /**
     * @brief The last character available in the font
     */
    uint8_t end_char;

    /**
     * @brief Space character width
     */
    uint8_t space_width;

    /**
     * @brief Pointer to array of char information
     */
    FontCharInfo* char_info;

    /**
     * @brief Pointer to generated array of character visual representation
     */
    uint8_t* data;
} FontInfo;

/**
 * @brief Allocates a new instance of \c FontInfo
 * @return pointer to allocated instance
 */
FontInfo* totp_font_info_alloc();

/**
 * @brief Disposes all the resources allocated by the given \c FontInfo instance
 * @param font_info instance to dispose
 */
void totp_font_info_free(FontInfo* font_info);