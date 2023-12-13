/* This file is part of the OpenMV project.
 * Copyright (c) 2013-2017 Ibrahim Abdelkader <iabdalkader@openmv.io> & Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 */

#ifndef __COLLECTIONS_H__
#define __COLLECTIONS_H__
#include <stdbool.h>
#include <stddef.h>
//////////
// lifo //
//////////

typedef struct lifo
{
  size_t len, size, data_len;
  char *data;
}
__attribute__((aligned(8))) lifo_t;

void lifo_alloc(lifo_t *ptr, size_t size, size_t data_len);
void lifo_alloc_all(lifo_t *ptr, size_t *size, size_t data_len);
void lifo_free(lifo_t *ptr);
void lifo_clear(lifo_t *ptr);
size_t lifo_size(lifo_t *ptr);
bool lifo_is_not_empty(lifo_t *ptr);
bool lifo_is_not_full(lifo_t *ptr);
void lifo_enqueue(lifo_t *ptr, void *data);
void lifo_dequeue(lifo_t *ptr, void *data);
void lifo_poke(lifo_t *ptr, void *data);
void lifo_peek(lifo_t *ptr, void *data);

#endif /* __COLLECTIONS_H__ */
