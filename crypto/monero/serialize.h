//
// Created by Dusan Klinec on 02/05/2018.
//

#ifndef TREZOR_XMR_SERIALIZE_H
#define TREZOR_XMR_SERIALIZE_H

#include <stddef.h>
#include <stdint.h>

int xmr_size_varint(uint64_t num);
int xmr_write_varint(uint8_t *buff, size_t buff_size, uint64_t num);
int xmr_read_varint(uint8_t *buff, size_t buff_size, uint64_t *val);

#endif  // TREZOR_XMR_SERIALIZE_H
