
#pragma once

#include "../../meal_pager_i.h"

char* encManchester(const char* bits, int mode);

void uint32ToBinaray(uint32_t number, char* str, int8_t length);

void reverse(char* str);

void customConcat(char* dest, const char* src);

//void meal_pager_retekess_t119_generate_pager(void* context, char* stationId, uint32_t pager);

//void meal_pager_retekess_t119_generate_station(void* context, uint32_t station);

void meal_pager_retekess_t119_generate_all(void* context);