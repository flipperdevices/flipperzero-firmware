#include "arraylist.h"

ArrayList* array_list_alloc() {
    ArrayList* newList = (ArrayList*)malloc(sizeof(ArrayList));
    if(newList == NULL) {
        FURI_LOG_D("DEBUG", "Memory allocation failed.\n");
        return NULL;
    }
    newList->array = NULL;
    newList->size = 0;
    newList->capacity = 0;
    return newList;
}

void array_list_append(ArrayList* list, uint8_t data) {
    if(list->size == list->capacity) {
        int newCapacity = (list->capacity == 0) ? 1 : list->capacity * 2;
        uint8_t* newArray = (uint8_t*)realloc(list->array, newCapacity * sizeof(uint8_t));
        if(newArray == NULL) {
            FURI_LOG_D("DEBUG", "Memory allocation failed.\n");
            return;
        }
        list->array = newArray;
        list->capacity = newCapacity;
    }
    list->array[list->size] = data;
    list->size++;
}

uint8_t array_list_get(ArrayList* list, int index) {
    if(index >= 0 && index < list->size) {
        return list->array[index];
    } else {
        FURI_LOG_D("DEBUG", "Index out of range.\n");
        return 0;
    }
}

char* array_list_to_string(ArrayList* list) {
    char* str = (char*)malloc((list->size * 4 + 1) * sizeof(char));
    if(str == NULL) {
        FURI_LOG_D("DEBUG", "Memory allocation failed.\n");
        return NULL;
    }
    str[0] = '\0';

    for(int i = 0; i < list->size; i++) {
        char temp[5];
        snprintf(temp, sizeof(temp), "%02x ", list->array[i]);
        strcat(str, temp);
    }
    return str;
}

void array_list_print(ArrayList* list) {
    if(list->size == 0) {
        FURI_LOG_D("DEBUG", "ArrayList is empty.\n");
        return;
    }
    for(int i = 0; i < list->size; i++) {
        FURI_LOG_D("DEBUG", "%u ", list->array[i]);
    }
    FURI_LOG_D("DEBUG", "\n");
}

void array_list_free(ArrayList* list) {
    free(list->array);
    free(list);
}
