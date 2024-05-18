#pragma once

#ifndef BMP_H
#define BMP_H

#define BMP_SIZE(w, h) ((h) * ((w) * 3 + (((w) * -3UL) & 3)) + 14 + 40)

void bmp_init(void* buf, long width, long height);
void bmp_set(void* buf, long x, long y, unsigned long color);
unsigned long bmp_encode(unsigned long color_hex);

#endif /* BMP_H */