#include "../app.h"

#if 0
/* Invert byte ordering. */
static void invert_nibbles(uint8_t *p, uint32_t len) {
    len *= 8;
    for (uint32_t j = 0; j < len; j += 4) {
        bool b0 = bitmap_get(p,len,j);
        bool b1 = bitmap_get(p,len,j+1);
        bool b2 = bitmap_get(p,len,j+2);
        bool b3 = bitmap_get(p,len,j+3);
        bitmap_set(p,len,j,b3);
        bitmap_set(p,len,j+1,b2);
        bitmap_set(p,len,j+2,b1);
        bitmap_set(p,len,j+3,b0);
    }
}
#endif

static bool decode(uint8_t *bits, uint32_t numbytes, uint32_t numbits, ProtoViewMsgInfo *info) {
    if (numbits < 32) return false;
    const char *sync_pattern = "01100110" "01100110" "10010110" "10010110";
    uint64_t off = bitmap_seek_bits(bits,numbytes,0,sync_pattern);
    if (off == BITMAP_SEEK_NOT_FOUND) return false;
    FURI_LOG_E(TAG, "Oregon2 prelude+sync found");

    off += 32; /* Skip preamble. */

    FURI_LOG_E(TAG, "Bits: %d%d%d%d",
        bitmap_get(bits,numbytes,off),
        bitmap_get(bits,numbytes,off+1),
        bitmap_get(bits,numbytes,off+2),
        bitmap_get(bits,numbytes,off+3));

    FURI_LOG_E(TAG, "Bits: %d%d%d%d",
        bitmap_get(bits,numbytes,off+4),
        bitmap_get(bits,numbytes,off+5),
        bitmap_get(bits,numbytes,off+6),
        bitmap_get(bits,numbytes,off+7));

    FURI_LOG_E(TAG, "Bits: %d%d%d%d",
        bitmap_get(bits,numbytes,off+8),
        bitmap_get(bits,numbytes,off+9),
        bitmap_get(bits,numbytes,off+10),
        bitmap_get(bits,numbytes,off+11));

    FURI_LOG_E(TAG, "Bits: %d%d%d%d",
        bitmap_get(bits,numbytes,off+12),
        bitmap_get(bits,numbytes,off+13),
        bitmap_get(bits,numbytes,off+14),
        bitmap_get(bits,numbytes,off+15));

    uint8_t buffer[8];
    uint32_t decoded =
        convert_from_line_code(buffer,sizeof(buffer),bits,numbytes,off,"1001","0110");
    FURI_LOG_E(TAG, "Oregon2 decoded bits: %lu", decoded);

    char temp[3] = {0}, deviceid[2] = {0};
    for (int j = 0; j < 64; j += 4) {
        uint8_t nib[1];
        nib[0] = 0;
        bitmap_set(nib,1,0,bitmap_get(buffer,8,j+0));
        bitmap_set(nib,1,1,bitmap_get(buffer,8,j+1));
        bitmap_set(nib,1,2,bitmap_get(buffer,8,j+2));
        bitmap_set(nib,1,3,bitmap_get(buffer,8,j+3));
        FURI_LOG_E(TAG, "Not inverted nibble[%d]: %x", j/4, (unsigned int)nib[0]);
        switch(j/4) {
        case 1: deviceid[0] |= nib[0]; break;
        case 0: deviceid[0] |= nib[0] << 4; break;
        case 3: deviceid[1] |= nib[0]; break;
        case 2: deviceid[1] |= nib[0] << 4; break;
        case 10: temp[0] = nib[0]; break;
        case 9: temp[1] = nib[0]; break;
        case 8: temp[2] = nib[0]; break;
        }
    }

    snprintf(info->name,sizeof(info->name),"%s","Oregon v2.1");
    snprintf(info->raw,sizeof(info->raw),"%08llX", *((uint64_t*)buffer));
    snprintf(info->info1,sizeof(info->info1),"Temp %d%d.%d",
        temp[0],temp[1],temp[2]);
    snprintf(info->info2,sizeof(info->info2),"ID %02X%02X",
        deviceid[0], deviceid[1]);
    return true;
}

ProtoViewDecoder Oregon2Decoder = {
    "Oregon2", decode
};
