#pragma once

#include "../meal_pager_i.h"

void meal_pager_blink_start_subghz(Meal_Pager* app);

void meal_pager_blink_start_compile(Meal_Pager* app);

void meal_pager_blink_stop(Meal_Pager* app);

void meal_pager_led_set_rgb(void* context, int red, int green, int blue);

void meal_pager_led_reset(void* context);
