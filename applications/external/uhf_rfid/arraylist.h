#pragma once
#include <furi.h>
#include <furi_hal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct ArrayList {
    uint8_t* array;
    int size;
    int capacity;
} ArrayList;

ArrayList* array_list_alloc();
void array_list_append(ArrayList* list, uint8_t data);
uint8_t array_list_get(ArrayList* list, int index);
char* array_list_to_string(ArrayList* list);
void array_list_print(ArrayList* list);
void array_list_free(ArrayList* list);
