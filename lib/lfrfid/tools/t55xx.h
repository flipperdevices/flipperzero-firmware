#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LFRFID_T55XX_BLOCK_COUNT 8

typedef struct {
    uint32_t block[LFRFID_T55XX_BLOCK_COUNT];
    uint32_t blocks_to_write;
} LFRFIDT55XX;

void t55xx_write(LFRFIDT55XX* data);

#ifdef __cplusplus
}
#endif