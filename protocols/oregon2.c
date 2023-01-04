#include "../app.h"

static bool decode(uint8_t *bits, uint32_t numbytes, uint32_t numbits, ProtoViewMsgInfo *info) {
    if (numbits < 32) return false;
    const char *sync_pattern = "01100110" "01100110" "10010110" "10010110";
    uint64_t off = bitmap_seek_bits(bits,numbytes,0,sync_pattern);
    if (off == BITMAP_SEEK_NOT_FOUND) return false;
    FURI_LOG_E(TAG, "Oregon2 prelude+sync found");

    off += 32; /* Skip preamble. */

    uint8_t buffer[8];
    uint32_t decoded =
        convert_from_line_code(buffer,sizeof(buffer),bits,numbytes,off,"1001","0110");
    FURI_LOG_E(TAG, "Oregon2 decoded bits: %lu", decoded);

    if (decoded < 11*4) return false; /* Minimum len to extract some data. */

    char temp[3] = {0}, deviceid[2] = {0}, hum[2] = {0};
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
        /* Fixme: take the temperature sign from nibble 11. */
        case 9: temp[1] = nib[0]; break;
        case 8: temp[2] = nib[0]; break;
        case 13: hum[0] = nib[0]; break;
        case 12: hum[1] = nib[0]; break;
        }
    }

    snprintf(info->name,sizeof(info->name),"%s","Oregon v2.1");
    /* The following line crashes the Flipper because of broken
     * snprintf() implementation. */
    if (0) snprintf(info->raw,sizeof(info->raw),"%08llX", *((uint64_t*)buffer));
    snprintf(info->info1,sizeof(info->info1),"Sensor ID %02X%02X",
        deviceid[0], deviceid[1]);
    snprintf(info->info2,sizeof(info->info2),"Temperature %d%d.%d",
        temp[0],temp[1],temp[2]);
    snprintf(info->info3,sizeof(info->info3),"Humidity %d%d",
        hum[0],hum[1]);
    return true;
}

ProtoViewDecoder Oregon2Decoder = {
    "Oregon2", decode
};
