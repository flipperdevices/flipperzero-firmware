#pragma once

#include <lib/nfc/protocols/mifare_classic.h>

bool magic_wupa();

bool magic_read_block(uint8_t block_num, MfClassicBlock* data);

bool magic_write_blk(uint8_t block_num, MfClassicBlock* data);

bool magic_wipe();
