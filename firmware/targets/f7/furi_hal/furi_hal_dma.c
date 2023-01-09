#include <furi.h>
#include <furi_hal_dma.h>

#define DMA_INSTANCE_COUNT 2
#define DMA_CHANNEL_COUNT 7
#define DMA_COUNT (DMA_INSTANCE_COUNT * DMA_CHANNEL_COUNT)

static const FuriHalDma dma_data[DMA_COUNT] = {
    {DMA1, LL_DMA_CHANNEL_1},
    {DMA1, LL_DMA_CHANNEL_2},
    {DMA1, LL_DMA_CHANNEL_3},
    {DMA1, LL_DMA_CHANNEL_4},
    {DMA1, LL_DMA_CHANNEL_5},
    {DMA1, LL_DMA_CHANNEL_6},
    {DMA1, LL_DMA_CHANNEL_7},
    {DMA2, LL_DMA_CHANNEL_1},
    {DMA2, LL_DMA_CHANNEL_2},
    {DMA2, LL_DMA_CHANNEL_3},
    {DMA2, LL_DMA_CHANNEL_4},
    {DMA2, LL_DMA_CHANNEL_5},
    {DMA2, LL_DMA_CHANNEL_6},
    {DMA2, LL_DMA_CHANNEL_7},
};

static bool dma_data_used[DMA_COUNT] = {0};

static const FuriHalInterruptId dma_irq_id[DMA_COUNT] = {
    FuriHalInterruptIdDma1Ch1,
    FuriHalInterruptIdDma1Ch2,
    FuriHalInterruptIdDma1Ch3,
    FuriHalInterruptIdDma1Ch4,
    FuriHalInterruptIdDma1Ch5,
    FuriHalInterruptIdDma1Ch6,
    FuriHalInterruptIdDma1Ch7,
    FuriHalInterruptIdDma2Ch1,
    FuriHalInterruptIdDma2Ch2,
    FuriHalInterruptIdDma2Ch3,
    FuriHalInterruptIdDma2Ch4,
    FuriHalInterruptIdDma2Ch5,
    FuriHalInterruptIdDma2Ch6,
    FuriHalInterruptIdDma2Ch7,
};

FuriMutex* furi_hal_dma_mutex = NULL;

static size_t furi_hal_dma_get_index(const FuriHalDma* instance) {
    size_t index = 0;
    for(uint32_t i = 0; i < DMA_COUNT; i++) {
        if(dma_data[i].dma == instance->dma && dma_data[i].channel == instance->channel) {
            index = i;
            break;
        }
    }
    return index;
}

static bool furi_hal_dma_is_used(const FuriHalDma* instance) {
    return dma_data_used[furi_hal_dma_get_index(instance)];
}

const FuriHalDma* furi_hal_dma_acquire_channel() {
    const FuriHalDma* dma = NULL;
    if(!furi_hal_dma_mutex) {
        furi_hal_dma_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    }

    furi_check(furi_mutex_acquire(furi_hal_dma_mutex, FuriWaitForever) == FuriStatusOk);
    for(uint32_t i = 0; i < DMA_COUNT; i++) {
        if(!dma_data_used[i]) {
            dma_data_used[i] = true;
            dma = &dma_data[i];
            break;
        }
    }
    furi_check(furi_mutex_release(furi_hal_dma_mutex) == FuriStatusOk);
    return dma;
};

void furi_hal_dma_release_channel(const FuriHalDma* dma) {
    furi_assert(dma);
    furi_check(furi_mutex_acquire(furi_hal_dma_mutex, FuriWaitForever) == FuriStatusOk);
    dma_data_used[furi_hal_dma_get_index(dma)] = false;
    furi_check(furi_mutex_release(furi_hal_dma_mutex) == FuriStatusOk);
}

FuriHalInterruptId furi_hal_dma_get_interrupt_id(const FuriHalDma* dma) {
    furi_assert(dma);
    return dma_irq_id[furi_hal_dma_get_index(dma)];
}

bool furi_hal_dma_get_half_transfer_flag(const FuriHalDma* dma) {
    furi_assert(dma);
    switch(dma->channel) {
    case LL_DMA_CHANNEL_1:
        return LL_DMA_IsActiveFlag_HT1(dma->dma);
    case LL_DMA_CHANNEL_2:
        return LL_DMA_IsActiveFlag_HT2(dma->dma);
    case LL_DMA_CHANNEL_3:
        return LL_DMA_IsActiveFlag_HT3(dma->dma);
    case LL_DMA_CHANNEL_4:
        return LL_DMA_IsActiveFlag_HT4(dma->dma);
    case LL_DMA_CHANNEL_5:
        return LL_DMA_IsActiveFlag_HT5(dma->dma);
    case LL_DMA_CHANNEL_6:
        return LL_DMA_IsActiveFlag_HT6(dma->dma);
    case LL_DMA_CHANNEL_7:
        return LL_DMA_IsActiveFlag_HT7(dma->dma);
    default:
        furi_crash(NULL);
    }
}

bool furi_hal_dma_get_transfer_complete_flag(const FuriHalDma* dma) {
    furi_assert(dma);
    switch(dma->channel) {
    case LL_DMA_CHANNEL_1:
        return LL_DMA_IsActiveFlag_TC1(dma->dma);
    case LL_DMA_CHANNEL_2:
        return LL_DMA_IsActiveFlag_TC2(dma->dma);
    case LL_DMA_CHANNEL_3:
        return LL_DMA_IsActiveFlag_TC3(dma->dma);
    case LL_DMA_CHANNEL_4:
        return LL_DMA_IsActiveFlag_TC4(dma->dma);
    case LL_DMA_CHANNEL_5:
        return LL_DMA_IsActiveFlag_TC5(dma->dma);
    case LL_DMA_CHANNEL_6:
        return LL_DMA_IsActiveFlag_TC6(dma->dma);
    case LL_DMA_CHANNEL_7:
        return LL_DMA_IsActiveFlag_TC7(dma->dma);
    default:
        furi_crash(NULL);
    }
}

bool furi_hal_dma_get_transfer_error_flag(const FuriHalDma* dma) {
    furi_assert(dma);
    switch(dma->channel) {
    case LL_DMA_CHANNEL_1:
        return LL_DMA_IsActiveFlag_TE1(dma->dma);
    case LL_DMA_CHANNEL_2:
        return LL_DMA_IsActiveFlag_TE2(dma->dma);
    case LL_DMA_CHANNEL_3:
        return LL_DMA_IsActiveFlag_TE3(dma->dma);
    case LL_DMA_CHANNEL_4:
        return LL_DMA_IsActiveFlag_TE4(dma->dma);
    case LL_DMA_CHANNEL_5:
        return LL_DMA_IsActiveFlag_TE5(dma->dma);
    case LL_DMA_CHANNEL_6:
        return LL_DMA_IsActiveFlag_TE6(dma->dma);
    case LL_DMA_CHANNEL_7:
        return LL_DMA_IsActiveFlag_TE7(dma->dma);
    default:
        furi_crash(NULL);
    }
}

void furi_hal_dma_clear_half_transfer_flag(const FuriHalDma* dma) {
    furi_assert(dma);
    switch(dma->channel) {
    case LL_DMA_CHANNEL_1:
        LL_DMA_ClearFlag_HT1(dma->dma);
        break;
    case LL_DMA_CHANNEL_2:
        LL_DMA_ClearFlag_HT2(dma->dma);
        break;
    case LL_DMA_CHANNEL_3:
        LL_DMA_ClearFlag_HT3(dma->dma);
        break;
    case LL_DMA_CHANNEL_4:
        LL_DMA_ClearFlag_HT4(dma->dma);
        break;
    case LL_DMA_CHANNEL_5:
        LL_DMA_ClearFlag_HT5(dma->dma);
        break;
    case LL_DMA_CHANNEL_6:
        LL_DMA_ClearFlag_HT6(dma->dma);
        break;
    case LL_DMA_CHANNEL_7:
        LL_DMA_ClearFlag_HT7(dma->dma);
        break;
    default:
        furi_crash(NULL);
    }
}

void furi_hal_dma_clear_transfer_complete_flag(const FuriHalDma* dma) {
    furi_assert(dma);
    switch(dma->channel) {
    case LL_DMA_CHANNEL_1:
        LL_DMA_ClearFlag_TC1(dma->dma);
        break;
    case LL_DMA_CHANNEL_2:
        LL_DMA_ClearFlag_TC2(dma->dma);
        break;
    case LL_DMA_CHANNEL_3:
        LL_DMA_ClearFlag_TC3(dma->dma);
        break;
    case LL_DMA_CHANNEL_4:
        LL_DMA_ClearFlag_TC4(dma->dma);
        break;
    case LL_DMA_CHANNEL_5:
        LL_DMA_ClearFlag_TC5(dma->dma);
        break;
    case LL_DMA_CHANNEL_6:
        LL_DMA_ClearFlag_TC6(dma->dma);
        break;
    case LL_DMA_CHANNEL_7:
        LL_DMA_ClearFlag_TC7(dma->dma);
        break;
    default:
        furi_crash(NULL);
    }
}

void furi_hal_dma_clear_transfer_error_flag(const FuriHalDma* dma) {
    furi_assert(dma);
    switch(dma->channel) {
    case LL_DMA_CHANNEL_1:
        LL_DMA_ClearFlag_TE1(dma->dma);
        break;
    case LL_DMA_CHANNEL_2:
        LL_DMA_ClearFlag_TE2(dma->dma);
        break;
    case LL_DMA_CHANNEL_3:
        LL_DMA_ClearFlag_TE3(dma->dma);
        break;
    case LL_DMA_CHANNEL_4:
        LL_DMA_ClearFlag_TE4(dma->dma);
        break;
    case LL_DMA_CHANNEL_5:
        LL_DMA_ClearFlag_TE5(dma->dma);
        break;
    case LL_DMA_CHANNEL_6:
        LL_DMA_ClearFlag_TE6(dma->dma);
        break;
    case LL_DMA_CHANNEL_7:
        LL_DMA_ClearFlag_TE7(dma->dma);
        break;
    default:
        furi_crash(NULL);
    }
}

void furi_hal_dma_init(const FuriHalDma* dma, LL_DMA_InitTypeDef* params) {
    FURI_CRITICAL_ENTER();
    furi_check(furi_hal_dma_is_used(dma) == true);
    LL_DMA_Init(dma->dma, dma->channel, params);
    FURI_CRITICAL_EXIT();
}

void furi_hal_dma_deinit(const FuriHalDma* dma) {
    FURI_CRITICAL_ENTER();
    furi_check(furi_hal_dma_is_used(dma) == true);
    LL_DMA_DeInit(dma->dma, dma->channel);
    FURI_CRITICAL_EXIT();
}