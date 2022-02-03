#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/** Get flash base address
 *
 * @return     pointer to flash base
 */
size_t furi_hal_flash_get_base();

/** Get flash read block size
 *
 * @return     size in bytes
 */
size_t furi_hal_flash_get_read_block_size();

/** Get flash write block size
 *
 * @return     size in bytes
 */
size_t furi_hal_flash_get_write_block_size();

/** Get flash page size
 *
 * @return     size in bytes
 */
size_t furi_hal_flash_get_page_size();

/** Get expected flash cycles count
 *
 * @return     count of erase-write operations
 */
size_t furi_hal_flash_get_cycles_count();

/** Get free flash end address
 *
 * @return     pointer to free region end
 */
const void* furi_hal_flash_get_free_end_address();

/** Get flash page number for address
 *
 * @return     page number, -1 for invalid address
 */
int16_t furi_hal_flash_get_page_number(size_t address);


/** Erase Flash
 *
 * @warning    locking operation with critical section, stales execution
 *
 * @param      page  The page to erase
 *
 * @return     true on success
 */
bool furi_hal_flash_erase(uint8_t page);

/** Write double word (64 bits)
 *
 * @warning locking operation with critical section, stales execution
 *
 * @param      address  destination address, must be double word aligned.
 * @param      data     data to write
 *
 * @return     true on success
 */
bool furi_hal_flash_write_dword(size_t address, uint64_t data);


bool furi_hal_flash_program_page(const uint8_t page, const uint8_t* data, uint16_t length);
