#include <stdlib.h>
#include <string.h>
#include "list.h"

struct List {
    int count;
    int capacity;
    int elementSize;
    char* data;
};

void list_add(List* list, void* element) {
    if(list->count >= list->capacity) return;
    memcpy(list->data + list->count * list->elementSize, element, list->elementSize);
    list->count++;
}

void list_remove_at(List* list, int index) {
    if(index < 0 || index >= list->count) return;
    memmove(
        list->data + index * list->elementSize,
        list->data + (index + 1) * list->elementSize,
        (list->count - index - 1) * list->elementSize);
    list->count--;
}

void* list_get_at(List* list, int index, void* result) {
    if(index < 0 || index >= list->count) return NULL;
    memcpy(result, list->data + index * list->elementSize, list->elementSize);
    return result;
}

void list_set_at(List* list, int index, void* element) {
    if(index < 0 || index >= list->count) return;
    memcpy(list->data + index * list->elementSize, element, list->elementSize);
}

int list_count(List* list) {
    return list->count;
}

int list_capacity(List* list) {
    return list->capacity;
}

void list_clear(List* list) {
    list->count = 0;
}

List* list_alloc(int capacity, int elementSize) {
    List* list = (List*)malloc(sizeof(List));
    list->data = (char*)malloc(capacity * elementSize);

    list->count = 0;
    list->capacity = capacity;
    list->elementSize = elementSize;

    return list;
}

void list_free(List* list) {
    free(list->data);
    free(list);
}