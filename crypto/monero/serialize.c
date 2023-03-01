//
// Created by Dusan Klinec on 02/05/2018.
//

#include "serialize.h"

int xmr_size_varint(uint64_t num) {
  int ctr = 1;
  while (num >= 0x80) {
    ++ctr;
    num >>= 7;
  }
  return ctr;
}

int xmr_write_varint(uint8_t *buff, size_t buff_size, uint64_t num) {
  unsigned ctr = 0;
  while (num >= 0x80 && ctr < buff_size) {
    *buff = (uint8_t)(((num)&0x7f) | 0x80);
    ++buff;
    ++ctr;
    num >>= 7;
  }

  /* writes the last one to dest */
  if (ctr < buff_size) {
    *buff = (uint8_t)num;
    ++ctr;
  }
  return ctr <= buff_size ? (int)ctr : -1;
}

int xmr_read_varint(uint8_t *buff, size_t buff_size, uint64_t *val) {
  unsigned read = 0;
  int finished_ok = 0;
  *val = 0;

  for (int shift = 0; read < buff_size; shift += 7, ++read) {
    uint8_t byte = buff[read];
    if ((byte == 0 && shift != 0) || (shift >= 63 && byte > 1)) {
      return -1;
    }

    *val |= (uint64_t)(byte & 0x7f) << shift;

    /* If there is no next */
    if ((byte & 0x80) == 0) {
      finished_ok = 1;
      break;
    }
  }
  return finished_ok ? (int)read + 1 : -2;
}
