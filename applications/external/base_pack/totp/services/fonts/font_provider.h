#pragma once

#include "font_info.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Gets total fonts available
 * @return total fonts available
 */
size_t totp_font_provider_get_fonts_count();

/**
 * @brief Load font with given index
 * @param font_index font index
 * @param[out] font_info font info to populate
 * @return \c true if font successfully load; \c false otherwise
 */
bool totp_font_provider_get_font(size_t font_index, FontInfo* font_info);