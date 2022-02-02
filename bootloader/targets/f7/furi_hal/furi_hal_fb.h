#pragma once

#include <u8g2.h>

void furi_hal_fb_init();

void furi_hal_fb_clear();

void furi_hal_fb_present();

u8g2_t* furi_hal_fb_get();