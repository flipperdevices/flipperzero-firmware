#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_SAMPLE_MEM 16384

typedef enum {
    SUMP_CMD_RESET = 0x00,
    SUMP_CMD_ARM = 0x01,
    SUMP_CMD_QUERY_ID = 0x02,
    SUMP_CMD_SELF_TEST = 0x03,
    SUMP_CMD_GET_METADATA = 0x04,
    SUMP_CMD_FINISH_NOW = 0x05,
    SUMP_CMD_XON = 0x11,
    SUMP_CMD_XOFF = 0x13,
    SUMP_CMD_SET_DIVIDER = 0x80,
    SUMP_CMD_SET_READ_DELAY_COUNT = 0x81,
    SUMP_CMD_SET_FLAGS = 0x82,
    SUMP_CMD_TRIGGER_MASK = 0xC0,
    SUMP_CMD_TRIGGER_VALUES = 0xC1,
    SUMP_CMD_TRIGGER_CONFIG = 0xC2,
} SumpCommands;

typedef struct {
    bool armed;
    uint8_t flags;
    uint32_t divider;
    uint32_t read_count;
    uint32_t delay_count;
    uint16_t trig_delay;
    uint16_t trig_config;
    uint32_t trig_mask;
    uint32_t trig_values;
    void (*tx_data)(void* ctx, uint8_t* data, size_t length);
    void* tx_data_ctx;
} Sump;

Sump* sump_alloc();

void sump_free(Sump* sump);

size_t sump_handle(Sump* sump, uint8_t* data, size_t length);
