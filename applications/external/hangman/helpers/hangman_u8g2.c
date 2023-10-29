/*
 * This is part of
 * Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)
 * Copyright (c) 2016, olikraus@gmail.com
 */

#include "hangman_u8g2.h"

static uint16_t hangman_font_get_word(const uint8_t* font, uint8_t offset) {
    uint16_t pos;
    font += offset;
    pos = hangman_pgm_read(font);
    font++;
    pos <<= 8;
    pos += hangman_pgm_read(font);
    return pos;
}

uint8_t hangman_font_decode_get_unsigned_bits(u8g2_font_decode_t* f, uint8_t cnt) {
    uint8_t val;
    uint8_t bit_pos = f->decode_bit_pos;
    uint8_t bit_pos_plus_cnt;

    val = hangman_pgm_read(f->decode_ptr);

    val >>= bit_pos;
    bit_pos_plus_cnt = bit_pos;
    bit_pos_plus_cnt += cnt;
    if(bit_pos_plus_cnt >= 8) {
        uint8_t s = 8;
        s -= bit_pos;
        f->decode_ptr++;
        val |= hangman_pgm_read(f->decode_ptr) << (s);
        bit_pos_plus_cnt -= 8;
    }
    val &= (1U << cnt) - 1;

    f->decode_bit_pos = bit_pos_plus_cnt;
    return val;
}

int8_t hangman_font_decode_get_signed_bits(u8g2_font_decode_t* f, uint8_t cnt) {
    int8_t v, d;
    v = (int8_t)hangman_font_decode_get_unsigned_bits(f, cnt);
    d = 1;
    cnt--;
    d <<= cnt;
    v -= d;
    return v;
}

static void hangman_font_setup_decode(u8g2_t* u8g2, const uint8_t* glyph_data) {
    u8g2_font_decode_t* decode = &(u8g2->font_decode);
    decode->decode_ptr = glyph_data;
    decode->decode_bit_pos = 0;

    decode->glyph_width =
        hangman_font_decode_get_unsigned_bits(decode, u8g2->font_info.bits_per_char_width);
    decode->glyph_height =
        hangman_font_decode_get_unsigned_bits(decode, u8g2->font_info.bits_per_char_height);

    decode->fg_color = u8g2->draw_color;
    decode->bg_color = (decode->fg_color == 0 ? 1 : 0);
}

const uint8_t* hangman_font_get_glyph_data(u8g2_t* u8g2, uint16_t encoding) {
    const uint8_t* font = u8g2->font;
    font += HANGMAN_FONT_DATA_STRUCT_SIZE;

    if(encoding <= 255) {
        if(encoding >= 'a') {
            font += u8g2->font_info.start_pos_lower_a;
        } else if(encoding >= 'A') {
            font += u8g2->font_info.start_pos_upper_A;
        }

        for(;;) {
            if(hangman_pgm_read(font + 1) == 0) break;
            if(hangman_pgm_read(font) == encoding) {
                return font + 2; /* skip encoding and glyph size */
            }
            font += hangman_pgm_read(font + 1);
        }
    }
#ifdef U8G2_WITH_UNICODE
    else {
        uint16_t e;
        const uint8_t* unicode_lookup_table;

        font += u8g2->font_info.start_pos_unicode;
        unicode_lookup_table = font;

        /* issue 596: search for the glyph start in the unicode lookup table */
        do {
            font += hangman_font_get_word(unicode_lookup_table, 0);
            e = hangman_font_get_word(unicode_lookup_table, 2);
            unicode_lookup_table += 4;
        } while(e < encoding);

        for(;;) {
            e = hangman_pgm_read(font);
            e <<= 8;
            e |= hangman_pgm_read(font + 1);

            if(e == 0) break;

            if(e == encoding) {
                return font + 3; /* skip encoding and glyph size */
            }
            font += hangman_pgm_read(font + 2);
        }
    }
#endif

    return NULL;
}

void hangman_get_glyph_data(u8x8_t* u8x8, uint8_t encoding, uint8_t* buf, uint8_t tile_offset) {
    uint8_t first, last, tiles, i;
    uint16_t offset;
    first = hangman_pgm_read(u8x8->font + 0);
    last = hangman_pgm_read(u8x8->font + 1);
    tiles = hangman_pgm_read(u8x8->font + 2); /* new 2019 format */
    tiles *= hangman_pgm_read(u8x8->font + 3); /* new 2019 format */

    /* get the glyph bitmap from the font */
    if(first <= encoding && encoding <= last) {
        offset = encoding;
        offset -= first;
        offset *= tiles; /* new 2019 format */
        offset += tile_offset; /* new 2019 format */
        offset *= 8;
        offset += 4; /* changed from 2 to 4, new 2019 format */
        for(i = 0; i < 8; i++) {
            buf[i] = hangman_pgm_read(u8x8->font + offset);
            offset++;
        }
    } else {
        for(i = 0; i < 8; i++) {
            buf[i] = 0;
        }
    }

    /* invert the bitmap if required */
    if(u8x8->is_font_inverse_mode) {
        for(i = 0; i < 8; i++) {
            buf[i] ^= 255;
        }
    }
}
int8_t hangman_GetGlyphWidth(u8g2_t* u8g2, uint16_t requested_encoding) {
    const uint8_t* glyph_data = hangman_font_get_glyph_data(u8g2, requested_encoding);
    if(glyph_data == NULL) return 0;

    hangman_font_setup_decode(u8g2, glyph_data);
    u8g2->glyph_x_offset =
        hangman_font_decode_get_signed_bits(&(u8g2->font_decode), u8g2->font_info.bits_per_char_x);
    hangman_font_decode_get_signed_bits(&(u8g2->font_decode), u8g2->font_info.bits_per_char_y);

    /* glyph width is here: u8g2->font_decode.glyph_width */

    return hangman_font_decode_get_signed_bits(
        &(u8g2->font_decode), u8g2->font_info.bits_per_delta_x);
}
