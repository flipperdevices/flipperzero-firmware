#ifndef MINESWEEPER_HAPTIC_H
#define MINESWEEPER_HAPTIC_H

#include <notification/notification_messages.h>

void mine_sweeper_play_happy_bump(void* context);

void mine_sweeper_play_oob_bump(void* context);

void mine_sweeper_play_end_bump(void* context);

void mine_sweeper_play_long_bump(void* context);

#endif
