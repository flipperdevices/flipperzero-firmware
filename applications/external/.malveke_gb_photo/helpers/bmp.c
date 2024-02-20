#include "bmp.h"
void bmp_init(void* buf, long width, long height) {
    long pad;
    unsigned long size;
    unsigned long uw = width;
    unsigned long uh = -height;
    unsigned char* p = (unsigned char*)buf;

#ifdef BMP_COMPAT
    uh = height;
#endif

    /* bfType */
    *p++ = 0x42;
    *p++ = 0x4D;

    /* bfSize */
    pad = (width * -3UL) & 3;
    size = height * (width * 3 + pad) + 14 + 40;
    *p++ = size >> 0;
    *p++ = size >> 8;
    *p++ = size >> 16;
    *p++ = size >> 24;

    /* bfReserved1 + bfReserved2 */
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;

    /* bfOffBits */
    *p++ = 0x36;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;

    /* biSize */
    *p++ = 0x28;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;

    /* biWidth */
    *p++ = uw >> 0;
    *p++ = uw >> 8;
    *p++ = uw >> 16;
    *p++ = uw >> 24;

    /* biHeight */
    *p++ = uh >> 0;
    *p++ = uh >> 8;
    *p++ = uh >> 16;
    *p++ = uh >> 24;

    /* biPlanes */
    *p++ = 0x01;
    *p++ = 0x00;

    /* biBitCount */
    *p++ = 0x18;
    *p++ = 0x00;

    /* biCompression */
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;

    /* biSizeImage */
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;

    /* biXPelsPerMeter */
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;

    /* biYPelsPerMeter */
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;

    /* biClrUsed */
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;

    /* biClrImportant */
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = 0x00;
    *p = 0x00;
}
void bmp_set(void* buf, long x, long y, unsigned long color) {
    unsigned char* p;
    unsigned char* hdr = (unsigned char*)buf;
    unsigned long width = (unsigned long)hdr[18] << 0 | (unsigned long)hdr[19] << 8 |
                          (unsigned long)hdr[20] << 16 | (unsigned long)hdr[21] << 24;
    long pad = (width * -3UL) & 3;
#ifdef BMP_COMPAT
    unsigned long height = (unsigned long)hdr[22] << 0 | (unsigned long)hdr[23] << 8 |
                           (unsigned long)hdr[24] << 16 | (unsigned long)hdr[25] << 24;
    y = height - y - 1;
#endif
    p = hdr + 14 + 40 + y * (width * 3 + pad) + x * 3;
    p[0] = color >> 0;
    p[1] = color >> 8;
    p[2] = color >> 16;
}
unsigned long bmp_encode(unsigned long color_hex) {
    unsigned char r = (color_hex >> 16) & 0xFF;
    unsigned char g = (color_hex >> 8) & 0xFF;
    unsigned char b = color_hex & 0xFF;

    return b | g << 8 | r << 16;
}
