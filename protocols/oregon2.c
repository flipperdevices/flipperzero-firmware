#include "../app.h"

static bool decode(uint8_t *bits, uint64_t numbits, ProtoViewMsgInfo *info) {
    const char *sync_pattern = "01100110" "01100110" "10010110" "10010110";
    uint64_t off = bitmap_seek_bits(bits,numbits,0,sync_pattern);
    if (off == BITMAP_SEEK_NOT_FOUND) return false;

    FURI_LOG_E(TAG, "Oregon2 prelude+sync found");
    snprintf(info->name,sizeof(info->name),"%s","Oregon v2.1");
    return true;
}

ProtoViewDecoder Oregon2Decoder = {
    "Oregon2", decode
};
