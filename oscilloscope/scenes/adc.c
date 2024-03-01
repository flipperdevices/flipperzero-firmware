#include "stm32wbxx_ll_adc.h"
#include <furi.h>
#define assert_param furi_assert

/* Check of parameters for configuration of ADC hierarchical scope:           */
/* common to several ADC instances.                                           */
#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_COMMON_CLOCK(__CLOCK__)                                                      \
    (((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV1) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV2) ||   \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV4) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV6) ||   \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV8) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV10) ||  \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV12) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV16) || \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV32) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV64) || \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV128) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV256))
#else
#define IS_LL_ADC_COMMON_CLOCK(__CLOCK__)                                                        \
    (((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV1) ||                                             \
     ((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV2) ||                                             \
     ((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV4) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV1) || \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV2) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV4) ||     \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV6) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV8) ||     \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV10) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV12) ||   \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV16) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV32) ||   \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV64) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV128) ||  \
     ((__CLOCK__) == LL_ADC_CLOCK_ASYNC_DIV256))
#endif /* ADC_SUPPORT_2_5_MSPS */

/* Check of parameters for configuration of ADC hierarchical scope:           */
/* ADC instance.                                                              */
#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_CLOCK(__CLOCK__)                   \
    (((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV4) || \
     ((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV2) || \
     ((__CLOCK__) == LL_ADC_CLOCK_SYNC_PCLK_DIV1) || ((__CLOCK__) == LL_ADC_CLOCK_ASYNC))

#endif /* ADC_SUPPORT_2_5_MSPS */
#define IS_LL_ADC_RESOLUTION(__RESOLUTION__)                                                      \
    (((__RESOLUTION__) == LL_ADC_RESOLUTION_12B) ||                                               \
     ((__RESOLUTION__) == LL_ADC_RESOLUTION_10B) || ((__RESOLUTION__) == LL_ADC_RESOLUTION_8B) || \
     ((__RESOLUTION__) == LL_ADC_RESOLUTION_6B))

#define IS_LL_ADC_DATA_ALIGN(__DATA_ALIGN__) \
    (((__DATA_ALIGN__) == LL_ADC_DATA_ALIGN_RIGHT) || ((__DATA_ALIGN__) == LL_ADC_DATA_ALIGN_LEFT))

#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_LOW_POWER(__LOW_POWER__)                                                  \
    (((__LOW_POWER__) == LL_ADC_LP_MODE_NONE) || ((__LOW_POWER__) == LL_ADC_LP_AUTOWAIT) || \
     ((__LOW_POWER__) == LL_ADC_LP_AUTOPOWEROFF) ||                                         \
     ((__LOW_POWER__) == LL_ADC_LP_AUTOWAIT_AUTOPOWEROFF))
#else
#define IS_LL_ADC_LOW_POWER(__LOW_POWER__) \
    (((__LOW_POWER__) == LL_ADC_LP_MODE_NONE) || ((__LOW_POWER__) == LL_ADC_LP_AUTOWAIT))
#endif /* ADC_SUPPORT_2_5_MSPS */
/* Check of parameters for configuration of ADC hierarchical scope:           */
/* ADC group regular                                                          */
#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_REG_TRIG_SOURCE(__REG_TRIG_SOURCE__)            \
    (((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_SOFTWARE) ||       \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_TRGO2) || \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH4) ||   \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_TRGO) ||  \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_CH4) ||   \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_CH3) ||   \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_EXTI_LINE11))
#else
#define IS_LL_ADC_REG_TRIG_SOURCE(__REG_TRIG_SOURCE__)            \
    (((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_SOFTWARE) ||       \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_TRGO) ||  \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_TRGO2) || \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH1) ||   \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH2) ||   \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM1_CH3) ||   \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_TRGO) ||  \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_TIM2_CH2) ||   \
     ((__REG_TRIG_SOURCE__) == LL_ADC_REG_TRIG_EXT_EXTI_LINE11))
#endif /* ADC_SUPPORT_2_5_MSPS */

#define IS_LL_ADC_REG_CONTINUOUS_MODE(__REG_CONTINUOUS_MODE__) \
    (((__REG_CONTINUOUS_MODE__) == LL_ADC_REG_CONV_SINGLE) ||  \
     ((__REG_CONTINUOUS_MODE__) == LL_ADC_REG_CONV_CONTINUOUS))

#define IS_LL_ADC_REG_DMA_TRANSFER(__REG_DMA_TRANSFER__)            \
    (((__REG_DMA_TRANSFER__) == LL_ADC_REG_DMA_TRANSFER_NONE) ||    \
     ((__REG_DMA_TRANSFER__) == LL_ADC_REG_DMA_TRANSFER_LIMITED) || \
     ((__REG_DMA_TRANSFER__) == LL_ADC_REG_DMA_TRANSFER_UNLIMITED))

#define IS_LL_ADC_REG_OVR_DATA_BEHAVIOR(__REG_OVR_DATA_BEHAVIOR__)     \
    (((__REG_OVR_DATA_BEHAVIOR__) == LL_ADC_REG_OVR_DATA_PRESERVED) || \
     ((__REG_OVR_DATA_BEHAVIOR__) == LL_ADC_REG_OVR_DATA_OVERWRITTEN))

#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_REG_SEQ_MODE(__REG_SEQ_MODE__)     \
    (((__REG_SEQ_MODE__) == LL_ADC_REG_SEQ_FIXED) || \
     ((__REG_SEQ_MODE__) == LL_ADC_REG_SEQ_CONFIGURABLE))
#endif /* ADC_SUPPORT_2_5_MSPS */

#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_REG_SEQ_SCAN_LENGTH(__REG_SEQ_SCAN_LENGTH__)           \
    (((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_DISABLE) ||       \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS))
#else
#define IS_LL_ADC_REG_SEQ_SCAN_LENGTH(__REG_SEQ_SCAN_LENGTH__)            \
    (((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_DISABLE) ||        \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS) ||  \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS) ||  \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS) ||  \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS) ||  \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS) ||  \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS) ||  \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS) ||  \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS) ||  \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS) || \
     ((__REG_SEQ_SCAN_LENGTH__) == LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS))
#endif /* ADC_SUPPORT_2_5_MSPS */
#if defined(ADC_SUPPORT_2_5_MSPS)
#define IS_LL_ADC_REG_SEQ_SCAN_DISCONT_MODE(__REG_SEQ_DISCONT_MODE__)  \
    (((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_DISABLE) || \
     ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_1RANK))
#else
#define IS_LL_ADC_REG_SEQ_SCAN_DISCONT_MODE(__REG_SEQ_DISCONT_MODE__)  \
    (((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_DISABLE) || \
     ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_1RANK) ||   \
     ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_2RANKS) ||  \
     ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_3RANKS) ||  \
     ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_4RANKS) ||  \
     ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_5RANKS) ||  \
     ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_6RANKS) ||  \
     ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_7RANKS) ||  \
     ((__REG_SEQ_DISCONT_MODE__) == LL_ADC_REG_SEQ_DISCONT_8RANKS))
#endif /* ADC_SUPPORT_2_5_MSPS */
/* Check of parameters for configuration of ADC hierarchical scope:           */
/* ADC group injected                                                         */
#define IS_LL_ADC_INJ_TRIG_SOURCE(__INJ_TRIG_SOURCE__)            \
    (((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_SOFTWARE) ||       \
     ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM1_TRGO) ||  \
     ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM1_TRGO2) || \
     ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM1_CH4) ||   \
     ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM2_TRGO) ||  \
     ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_TIM2_CH1) ||   \
     ((__INJ_TRIG_SOURCE__) == LL_ADC_INJ_TRIG_EXT_EXTI_LINE15))

#define IS_LL_ADC_INJ_TRIG_EXT_EDGE(__INJ_TRIG_EXT_EDGE__)       \
    (((__INJ_TRIG_EXT_EDGE__) == LL_ADC_INJ_TRIG_EXT_RISING) ||  \
     ((__INJ_TRIG_EXT_EDGE__) == LL_ADC_INJ_TRIG_EXT_FALLING) || \
     ((__INJ_TRIG_EXT_EDGE__) == LL_ADC_INJ_TRIG_EXT_RISINGFALLING))

#define IS_LL_ADC_INJ_TRIG_AUTO(__INJ_TRIG_AUTO__)           \
    (((__INJ_TRIG_AUTO__) == LL_ADC_INJ_TRIG_INDEPENDENT) || \
     ((__INJ_TRIG_AUTO__) == LL_ADC_INJ_TRIG_FROM_GRP_REGULAR))

#define IS_LL_ADC_INJ_SEQ_SCAN_LENGTH(__INJ_SEQ_SCAN_LENGTH__)           \
    (((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_DISABLE) ||       \
     ((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_ENABLE_2RANKS) || \
     ((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_ENABLE_3RANKS) || \
     ((__INJ_SEQ_SCAN_LENGTH__) == LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS))

#define IS_LL_ADC_INJ_SEQ_SCAN_DISCONT_MODE(__INJ_SEQ_DISCONT_MODE__)  \
    (((__INJ_SEQ_DISCONT_MODE__) == LL_ADC_INJ_SEQ_DISCONT_DISABLE) || \
     ((__INJ_SEQ_DISCONT_MODE__) == LL_ADC_INJ_SEQ_DISCONT_1RANK))

ErrorStatus LL_ADC_Init(ADC_TypeDef* ADCx, const LL_ADC_InitTypeDef* ADC_InitStruct) {
    ErrorStatus status = SUCCESS;

    /* Check the parameters */
    assert_param(IS_ADC_ALL_INSTANCE(ADCx));

#if defined(ADC_SUPPORT_2_5_MSPS)
    assert_param(IS_LL_ADC_CLOCK(ADC_InitStruct->Clock));
#endif /* ADC_SUPPORT_2_5_MSPS */
    assert_param(IS_LL_ADC_RESOLUTION(ADC_InitStruct->Resolution));
    assert_param(IS_LL_ADC_DATA_ALIGN(ADC_InitStruct->DataAlignment));
    assert_param(IS_LL_ADC_LOW_POWER(ADC_InitStruct->LowPowerMode));

    /* Note: Hardware constraint (refer to description of this function):       */
    /*       ADC instance must be disabled.                                     */
    if(LL_ADC_IsEnabled(ADCx) == 0UL) {
        /* Configuration of ADC hierarchical scope:                               */
        /*  - ADC instance                                                        */
        /*    - Set ADC data resolution                                           */
        /*    - Set ADC conversion data alignment                                 */
        /*    - Set ADC low power mode                                            */
#if defined(ADC_SUPPORT_2_5_MSPS)
        MODIFY_REG(
            ADCx->CFGR1,
            ADC_CFGR1_RES | ADC_CFGR1_ALIGN | ADC_CFGR1_WAIT | ADC_CFGR1_AUTOFF,
            ADC_InitStruct->Resolution | ADC_InitStruct->DataAlignment |
                ADC_InitStruct->LowPowerMode);

        MODIFY_REG(ADCx->CFGR2, ADC_CFGR2_CKMODE, ADC_InitStruct->Clock);
#else
        MODIFY_REG(
            ADCx->CFGR,
            ADC_CFGR_RES | ADC_CFGR_ALIGN | ADC_CFGR_AUTDLY,
            ADC_InitStruct->Resolution | ADC_InitStruct->DataAlignment |
                ADC_InitStruct->LowPowerMode);
#endif /* ADC_SUPPORT_2_5_MSPS */
    } else {
        /* Initialization error: ADC instance is not disabled. */
        status = ERROR;
    }
    return status;
}

ErrorStatus LL_ADC_REG_Init(ADC_TypeDef* ADCx, const LL_ADC_REG_InitTypeDef* ADC_REG_InitStruct) {
    ErrorStatus status = SUCCESS;

    /* Check the parameters */
    assert_param(IS_ADC_ALL_INSTANCE(ADCx));
    assert_param(IS_LL_ADC_REG_TRIG_SOURCE(ADC_REG_InitStruct->TriggerSource));
#if defined(ADC_SUPPORT_2_5_MSPS)
    if(LL_ADC_REG_GetSequencerConfigurable(ADCx) != LL_ADC_REG_SEQ_FIXED) {
        assert_param(IS_LL_ADC_REG_SEQ_SCAN_LENGTH(ADC_REG_InitStruct->SequencerLength));
    }
    if((LL_ADC_REG_GetSequencerConfigurable(ADCx) == LL_ADC_REG_SEQ_FIXED) ||
       (ADC_REG_InitStruct->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE)) {
        assert_param(IS_LL_ADC_REG_SEQ_SCAN_DISCONT_MODE(ADC_REG_InitStruct->SequencerDiscont));

        /* ADC group regular continuous mode and discontinuous mode                 */
        /* can not be enabled simultenaeously                                       */
        assert_param(
            (ADC_REG_InitStruct->ContinuousMode == LL_ADC_REG_CONV_SINGLE) ||
            (ADC_REG_InitStruct->SequencerDiscont == LL_ADC_REG_SEQ_DISCONT_DISABLE));
    }
#else
    assert_param(IS_LL_ADC_REG_SEQ_SCAN_LENGTH(ADC_REG_InitStruct->SequencerLength));
    if(ADC_REG_InitStruct->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE) {
        assert_param(IS_LL_ADC_REG_SEQ_SCAN_DISCONT_MODE(ADC_REG_InitStruct->SequencerDiscont));

        /* ADC group regular continuous mode and discontinuous mode                 */
        /* can not be enabled simultenaeously                                       */
        assert_param(
            (ADC_REG_InitStruct->ContinuousMode == LL_ADC_REG_CONV_SINGLE) ||
            (ADC_REG_InitStruct->SequencerDiscont == LL_ADC_REG_SEQ_DISCONT_DISABLE));
    }
#endif /* ADC_SUPPORT_2_5_MSPS */
    assert_param(IS_LL_ADC_REG_CONTINUOUS_MODE(ADC_REG_InitStruct->ContinuousMode));
    assert_param(IS_LL_ADC_REG_DMA_TRANSFER(ADC_REG_InitStruct->DMATransfer));
    assert_param(IS_LL_ADC_REG_OVR_DATA_BEHAVIOR(ADC_REG_InitStruct->Overrun));

    /* Note: Hardware constraint (refer to description of this function):       */
    /*       ADC instance must be disabled.                                     */
    if(LL_ADC_IsEnabled(ADCx) == 0UL) {
        /* Configuration of ADC hierarchical scope:                               */
        /*  - ADC group regular                                                   */
        /*    - Set ADC group regular trigger source                              */
        /*    - Set ADC group regular sequencer length                            */
        /*    - Set ADC group regular sequencer discontinuous mode                */
        /*    - Set ADC group regular continuous mode                             */
        /*    - Set ADC group regular conversion data transfer: no transfer or    */
        /*      transfer by DMA, and DMA requests mode                            */
        /*    - Set ADC group regular overrun behavior                            */
        /* Note: On this STM32 series, ADC trigger edge is set to value 0x0 by     */
        /*       setting of trigger source to SW start.                           */
#if defined(ADC_SUPPORT_2_5_MSPS)
        if((LL_ADC_REG_GetSequencerConfigurable(ADCx) == LL_ADC_REG_SEQ_FIXED) ||
           (ADC_REG_InitStruct->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE)) {
            /* Case of sequencer mode fixed
               or sequencer length >= 2 ranks with sequencer mode fully configurable:
               discontinuous mode configured */
            MODIFY_REG(
                ADCx->CFGR1,
                ADC_CFGR1_EXTSEL | ADC_CFGR1_EXTEN | ADC_CFGR1_DISCEN | ADC_CFGR1_CONT |
                    ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG | ADC_CFGR1_OVRMOD,
                ADC_REG_InitStruct->TriggerSource | ADC_REG_InitStruct->SequencerDiscont |
                    ADC_REG_InitStruct->ContinuousMode | ADC_REG_InitStruct->DMATransfer |
                    ADC_REG_InitStruct->Overrun);
        } else {
            /* Case of sequencer mode fully configurable
               and sequencer length 1 rank (sequencer disabled):
               discontinuous mode discarded (fixed to disable) */
            MODIFY_REG(
                ADCx->CFGR1,
                ADC_CFGR1_EXTSEL | ADC_CFGR1_EXTEN | ADC_CFGR1_DISCEN | ADC_CFGR1_CONT |
                    ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG | ADC_CFGR1_OVRMOD,
                ADC_REG_InitStruct->TriggerSource | LL_ADC_REG_SEQ_DISCONT_DISABLE |
                    ADC_REG_InitStruct->ContinuousMode | ADC_REG_InitStruct->DMATransfer |
                    ADC_REG_InitStruct->Overrun);
        }
#else
        if(ADC_REG_InitStruct->SequencerLength != LL_ADC_REG_SEQ_SCAN_DISABLE) {
            MODIFY_REG(
                ADCx->CFGR,
                ADC_CFGR_EXTSEL | ADC_CFGR_EXTEN | ADC_CFGR_DISCEN | ADC_CFGR_DISCNUM |
                    ADC_CFGR_CONT | ADC_CFGR_DMAEN | ADC_CFGR_DMACFG | ADC_CFGR_OVRMOD,
                ADC_REG_InitStruct->TriggerSource | ADC_REG_InitStruct->SequencerDiscont |
                    ADC_REG_InitStruct->ContinuousMode | ADC_REG_InitStruct->DMATransfer |
                    ADC_REG_InitStruct->Overrun);
        } else {
            MODIFY_REG(
                ADCx->CFGR,
                ADC_CFGR_EXTSEL | ADC_CFGR_EXTEN | ADC_CFGR_DISCEN | ADC_CFGR_DISCNUM |
                    ADC_CFGR_CONT | ADC_CFGR_DMAEN | ADC_CFGR_DMACFG | ADC_CFGR_OVRMOD,
                ADC_REG_InitStruct->TriggerSource | LL_ADC_REG_SEQ_DISCONT_DISABLE |
                    ADC_REG_InitStruct->ContinuousMode | ADC_REG_InitStruct->DMATransfer |
                    ADC_REG_InitStruct->Overrun);
        }
#endif /* ADC_SUPPORT_2_5_MSPS */

        /* Set ADC group regular sequencer length and scan direction */
#if defined(ADC_SUPPORT_2_5_MSPS)
        if(LL_ADC_REG_GetSequencerConfigurable(ADCx) != LL_ADC_REG_SEQ_FIXED) {
            LL_ADC_REG_SetSequencerLength(ADCx, ADC_REG_InitStruct->SequencerLength);
        }
#else
        LL_ADC_REG_SetSequencerLength(ADCx, ADC_REG_InitStruct->SequencerLength);
#endif /* ADC_SUPPORT_2_5_MSPS */
    } else {
        /* Initialization error: ADC instance is not disabled. */
        status = ERROR;
    }
    return status;
}

ErrorStatus LL_ADC_CommonInit(
    ADC_Common_TypeDef* ADCxy_COMMON,
    const LL_ADC_CommonInitTypeDef* ADC_CommonInitStruct) {
    ErrorStatus status = SUCCESS;

    /* Check the parameters */
    assert_param(IS_ADC_COMMON_INSTANCE(ADCxy_COMMON));
    assert_param(IS_LL_ADC_COMMON_CLOCK(ADC_CommonInitStruct->CommonClock));

    /* Note: Hardware constraint (refer to description of functions             */
    /*       "LL_ADC_SetCommonXXX()":                                           */
    /*       On this STM32 series, setting of these features is conditioned to   */
    /*       ADC state:                                                         */
    /*       All ADC instances of the ADC common group must be disabled.        */
    if(__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE(ADCxy_COMMON) == 0UL) {
        /* Configuration of ADC hierarchical scope:                               */
        /*  - common to several ADC                                               */
        /*    (all ADC instances belonging to the same ADC common instance)       */
        /*    - Set ADC clock (conversion clock)                                  */
#if defined(ADC_MULTIMODE_SUPPORT)
        if(ADC_CommonInitStruct->Multimode != LL_ADC_MULTI_INDEPENDENT) {
            MODIFY_REG(
                ADCxy_COMMON->CCR,
                ADC_CCR_CKMODE | ADC_CCR_PRESC | ADC_CCR_DUAL | ADC_CCR_MDMA | ADC_CCR_DELAY,
                ADC_CommonInitStruct->CommonClock | ADC_CommonInitStruct->Multimode |
                    ADC_CommonInitStruct->MultiDMATransfer |
                    ADC_CommonInitStruct->MultiTwoSamplingDelay);
        } else {
            MODIFY_REG(
                ADCxy_COMMON->CCR,
                ADC_CCR_CKMODE | ADC_CCR_PRESC | ADC_CCR_DUAL | ADC_CCR_MDMA | ADC_CCR_DELAY,
                ADC_CommonInitStruct->CommonClock | LL_ADC_MULTI_INDEPENDENT);
        }
#else
        LL_ADC_SetCommonClock(ADCxy_COMMON, ADC_CommonInitStruct->CommonClock);
#endif
    } else {
        /* Initialization error: One or several ADC instances belonging to        */
        /* the same ADC common instance are not disabled.                         */
        status = ERROR;
    }

    return status;
}
