#include <stdlib.h>

typedef struct List List;

void list_add(List* list, void* element);
void list_remove_at(List* list, int index);
void* list_get_at(List* list, int index, void* result);
void list_set_at(List* list, int index, void* element);
int list_count(List* list);
int list_capacity(List* list);
void list_clear(List* list);

List* list_alloc(int maxCapacity, int elementSize);
void list_free(List* list);