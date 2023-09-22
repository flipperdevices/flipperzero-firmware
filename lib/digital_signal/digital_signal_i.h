#include "digital_signal.h"

#include <stm32wbxx_ll_dma.h>

#define F_TIM (64000000.0)
#define T_TIM 1562 /* 15.625 ns *100 */
#define T_TIM_DIV2 781 /* 15.625 ns / 2 *100 */

typedef struct {
    uint64_t factor;
    uint32_t reload_reg_entries;
    uint32_t reload_reg_remainder;
    uint32_t gpio_buff[2];
    const GpioPin* gpio;
    LL_DMA_InitTypeDef dma_config_gpio;
    LL_DMA_InitTypeDef dma_config_timer;
} DigitalSignalInternals;

/* and a public one for accessing user-side fields */
struct DigitalSignal {
    bool start_level;
    uint32_t edge_cnt;
    uint32_t edges_max_cnt;
    uint32_t* edge_timings;
    uint32_t* reload_reg_buff; /* internal, but used by unit tests */
    DigitalSignalInternals* internals;
};

void digital_signal_prepare_arr(DigitalSignal* signal);
