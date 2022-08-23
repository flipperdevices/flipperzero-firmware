/**
 * @file furi_hal_memory.h
 * Memory HAL API
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Init memory pool manager
 */
void furi_hal_memory_init();

/**
 * @brief Allocate memory from separate memory pool. That memory can't be freed.
 * 
 * @param size 
 * @return void* 
 */
void* furi_hal_memory_alloc(size_t size);

#ifdef __cplusplus
}
#endif