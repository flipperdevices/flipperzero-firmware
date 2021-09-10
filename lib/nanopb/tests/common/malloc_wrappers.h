#include <stdlib.h>

void* malloc_with_check(size_t size);
void free_with_check(void *mem);
void* realloc_with_check(void *ptr, size_t size);
size_t get_alloc_count();
size_t get_allocation_size(const void *mem);
size_t get_alloc_bytes();
void set_max_alloc_bytes(size_t max_bytes);
size_t get_max_alloc_bytes();
