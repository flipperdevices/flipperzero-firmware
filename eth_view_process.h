#pragma once

#include "eth_worker.h"
#include <gui/gui.h>

#define SCREEN_SYMBOLS_WIDTH 30

EthViewProcess* ethernet_view_process_malloc(EthWorkerProcess type);
void ethernet_view_process_free(EthViewProcess* evp);

void ethernet_view_process_draw(EthViewProcess* process, Canvas* canvas);
void ethernet_view_process_keyevent(EthViewProcess* process, InputKey key);
void ethernet_view_process_print(EthViewProcess* process, const char* str);
void ethernet_view_process_move(EthViewProcess* process, int8_t shift);
void evp_printf(EthViewProcess* process, const char* format, ...);

typedef struct EthViewProcessLine {
    char data[SCREEN_SYMBOLS_WIDTH];
} EthViewProcessLine;

struct EthViewProcess {
    EthViewProcessLine* fifo;
    uint8_t strings_cnt;
    uint8_t x;
    uint8_t y;
    uint8_t carriage;
    uint8_t position;
    uint8_t autofill;
    uint8_t editing;
    EthWorkerProcess type;
    void* draw_struct;
};

typedef struct EthViewDrawInit {
    uint8_t mac[6];
    uint8_t current_octet;
} EthViewDrawInit;

typedef enum {
    EthViewDrawStaticModeIp,
    EthViewDrawStaticModeMask,
    EthViewDrawStaticModeGateway,
    EthViewDrawStaticModeDNS
} EthViewDrawStaticMode;
typedef struct EthViewDrawStatic {
    uint8_t ip[4];
    uint8_t mask[4];
    uint8_t gateway[4];
    uint8_t dns[4];
    EthViewDrawStaticMode current_mode;
    uint8_t current_digit;
    uint8_t editing;
} EthViewDrawStatic;

typedef struct EthViewDrawPing {
    uint8_t ip[4];
    uint8_t current_digit;
} EthViewDrawPing;