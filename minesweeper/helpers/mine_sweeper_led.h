#ifndef MINESWEEPER_LED_H
#define MINESWEEPER_LED_H

void mine_sweeper_led_set_rgb(void* context, int red, int green, int blue);

void mine_sweeper_led_blink_red(void* context);

void mine_sweeper_led_blink_magenta(void* context);

void mine_sweeper_led_blink_cyan(void* context);

void mine_sweeper_led_reset(void* context);

#endif
