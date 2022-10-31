#include "magic.h"

bool magic_wupa() {
    return true;
}

bool magic_read_block(uint8_t block_num, MfClassicBlock* data) {
    furi_assert(data);
    UNUSED(block_num);

    return true;
}

bool magic_write_blk(uint8_t block_num, MfClassicBlock* data) {
    furi_assert(data);
    UNUSED(block_num);

    return true;
}

bool magic_wipe() {
    return true;
}

