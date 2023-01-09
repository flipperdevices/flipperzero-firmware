/**
 * @file furi_hal_dma.h
 * Furi HAL DMA API
 */
#pragma once
#include <furi.h>
#include <stm32wbxx_ll_dma.h>
#include <furi_hal_interrupt.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @brief DMA instance
*/
typedef struct {
    DMA_TypeDef* dma;
    uint32_t channel;
} FuriHalDma;

#define DMA_UNPACK(dma_instance) (dma_instance)->dma, (dma_instance)->channel

/**
 * @brief Acquire free DMA channel
 * Can return NULL if no free channel
 * 
 * @return const FuriHalDma* DMA instance, NULL if no free channel
 */
FURI_WARN_UNUSED const FuriHalDma* furi_hal_dma_acquire_channel();

/**
 * @brief Release DMA channel
 * 
 * @param dma DMA instance
 */
void furi_hal_dma_release_channel(const FuriHalDma* dma);

/**
 * @brief Get DMA IRQ ID
 * 
 * @param dma DMA instance
 * @return FuriHalInterruptId IRQ ID
 */
FuriHalInterruptId furi_hal_dma_get_interrupt_id(const FuriHalDma* dma);

/**
 * @brief Get DMA half transfer flag
 * 
 * @param dma 
 * @return bool 
 */
bool furi_hal_dma_get_half_transfer_flag(const FuriHalDma* dma);

/**
 * @brief Get DMA transfer complete flag
 * 
 * @param dma 
 * @return bool 
 */
bool furi_hal_dma_get_transfer_complete_flag(const FuriHalDma* dma);

/**
 * @brief Get DMA transfer error flag
 * 
 * @param dma 
 * @return bool 
 */
bool furi_hal_dma_get_transfer_error_flag(const FuriHalDma* dma);

/**
 * @brief Clear DMA half transfer flag
 * 
 * @param dma 
 */
void furi_hal_dma_clear_half_transfer_flag(const FuriHalDma* dma);

/**
 * @brief Clear DMA transfer complete flag
 * 
 * @param dma 
 */
void furi_hal_dma_clear_transfer_complete_flag(const FuriHalDma* dma);

/**
 * @brief Clear DMA transfer error flag
 * 
 * @param dma 
 */
void furi_hal_dma_clear_transfer_error_flag(const FuriHalDma* dma);

/**
 * @brief Init DMA instance
 * 
 * @param dma 
 * @param params 
 */
void furi_hal_dma_init(const FuriHalDma* dma, LL_DMA_InitTypeDef* params);

/**
 * @brief Deinit DMA instance
 * 
 * @param dma 
 */
void furi_hal_dma_deinit(const FuriHalDma* dma);

#ifdef __cplusplus
}
#endif