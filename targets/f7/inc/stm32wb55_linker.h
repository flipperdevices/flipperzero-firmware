/**
 * @file stm32wb55_linker.h
 *
 * Linker defined symbols. Used in various part of firmware to understand
 * hardware boundaries.
 * 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t _stack_end; /**< end of stack */
extern const uint8_t _stack_size; /**< stack size */

extern const uint8_t _sidata; /**< data initial value start */
extern const uint8_t _sdata; /**< data start */
extern const uint8_t _edata; /**< data end */

extern const uint8_t _sbss; /**< bss start */
extern const uint8_t _ebss; /**< bss end */

extern const uint8_t _sMB_MEM2; /**< RAM2a start */
extern const uint8_t _eMB_MEM2; /**< RAM2a end */

extern const uint8_t __heap_start__; /**< RAM1 Heap start */
extern const uint8_t __heap_end__; /**< RAM1 Heap end */

extern const uint8_t __free_flash_start__; /**< Free Flash space start */

#ifdef __cplusplus
}
#endif
