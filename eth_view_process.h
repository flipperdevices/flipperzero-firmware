#pragma once

#include "eth_worker.h"
#include <gui/gui.h>

#define SCREEN_SYMBOLS_WIDTH 30
#define SCREEN_STRINGS_COUNT 40

EthViewProcess* ethernet_view_process_malloc();
void ethernet_view_process_free(EthViewProcess* evp);

void ethernet_view_process_draw(EthViewProcess* process, Canvas* canvas);
void ethernet_view_process_print(EthViewProcess* process, const char* str);
void ethernet_view_process_move(EthViewProcess* process, int8_t shift);
