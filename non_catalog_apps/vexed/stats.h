#pragma once

#include "common.h"

Stats* alloc_stats();
void free_stats(Stats* stats);
void update_board_stats(PlayGround* pg, Stats* stats);