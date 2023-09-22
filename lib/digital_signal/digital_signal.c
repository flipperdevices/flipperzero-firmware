#include "digital_signal_i.h"

#include <furi.h>
#include <furi_hal_bus.h>
#include <furi_hal_resources.h>

#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_tim.h>

/* must be on bank B */
// For debugging purposes use `--extra-define=DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN=gpio_ext_pb3` fbt option

#define TAG "DigitalSignal"

DigitalSignal* digital_signal_alloc(uint32_t max_edges_cnt) {
    DigitalSignal* signal = malloc(sizeof(DigitalSignal));
    signal->start_level = true;
    signal->edges_max_cnt = max_edges_cnt;
    signal->edge_timings = malloc(signal->edges_max_cnt * sizeof(uint32_t));
    signal->edge_cnt = 0;
    signal->reload_reg_buff = malloc(signal->edges_max_cnt * sizeof(uint32_t));

    signal->internals = malloc(sizeof(DigitalSignalInternals));
    DigitalSignalInternals* internals = signal->internals;

    internals->factor = 1024 * 1024;

    internals->dma_config_gpio.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    internals->dma_config_gpio.Mode = LL_DMA_MODE_CIRCULAR;
    internals->dma_config_gpio.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    internals->dma_config_gpio.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    internals->dma_config_gpio.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    internals->dma_config_gpio.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    internals->dma_config_gpio.NbData = 2;
    internals->dma_config_gpio.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    internals->dma_config_gpio.Priority = LL_DMA_PRIORITY_VERYHIGH;

    internals->dma_config_timer.PeriphOrM2MSrcAddress = (uint32_t) & (TIM2->ARR);
    internals->dma_config_timer.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    internals->dma_config_timer.Mode = LL_DMA_MODE_NORMAL;
    internals->dma_config_timer.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    internals->dma_config_timer.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    internals->dma_config_timer.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    internals->dma_config_timer.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    internals->dma_config_timer.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    internals->dma_config_timer.Priority = LL_DMA_PRIORITY_HIGH;

    return signal;
}

void digital_signal_free(DigitalSignal* signal) {
    furi_assert(signal);

    free(signal->edge_timings);
    free(signal->reload_reg_buff);
    free(signal->internals);
    free(signal);
}

bool digital_signal_append(DigitalSignal* signal_a, DigitalSignal* signal_b) {
    furi_assert(signal_a);
    furi_assert(signal_b);

    if(signal_a->edges_max_cnt < signal_a->edge_cnt + signal_b->edge_cnt) {
        return false;
    }
    /* in case there are no edges in our target signal, the signal to append makes the rules */
    if(!signal_a->edge_cnt) {
        signal_a->start_level = signal_b->start_level;
    }
    bool end_level = signal_a->start_level;
    if(signal_a->edge_cnt) {
        end_level = signal_a->start_level ^ !(signal_a->edge_cnt % 2);
    }
    uint8_t start_copy = 0;
    if(end_level == signal_b->start_level) {
        if(signal_a->edge_cnt) {
            signal_a->edge_timings[signal_a->edge_cnt - 1] += signal_b->edge_timings[0];
            start_copy += 1;
        } else {
            signal_a->edge_timings[signal_a->edge_cnt] += signal_b->edge_timings[0];
        }
    }

    for(size_t i = 0; i < signal_b->edge_cnt - start_copy; i++) {
        signal_a->edge_timings[signal_a->edge_cnt + i] = signal_b->edge_timings[start_copy + i];
    }
    signal_a->edge_cnt += signal_b->edge_cnt - start_copy;

    return true;
}

bool digital_signal_get_start_level(const DigitalSignal* signal) {
    furi_assert(signal);

    return signal->start_level;
}

void digital_signal_set_start_level(DigitalSignal* signal, bool level) {
    furi_assert(signal);

    signal->start_level = level;
}

uint32_t digital_signal_get_edges_cnt(const DigitalSignal* signal) {
    furi_assert(signal);

    return signal->edge_cnt;
}

void digital_signal_add_edge(DigitalSignal* signal, uint32_t ticks) {
    furi_assert(signal);
    furi_assert(signal->edge_cnt < signal->edges_max_cnt);

    signal->edge_timings[signal->edge_cnt++] = ticks;
}

void digital_signal_add_pulse(DigitalSignal* signal, uint32_t ticks, bool level) {
    furi_assert(signal);
    furi_assert(signal->edge_cnt < signal->edges_max_cnt);

    /* virgin signal? add it as the only level */
    if(signal->edge_cnt == 0) {
        signal->start_level = level;
        signal->edge_timings[signal->edge_cnt++] = ticks;
    } else {
        bool end_level = signal->start_level ^ !(signal->edge_cnt % 2);

        if(level != end_level) {
            signal->edge_timings[signal->edge_cnt++] = ticks;
        } else {
            signal->edge_timings[signal->edge_cnt - 1] += ticks;
        }
    }
}

uint32_t digital_signal_get_edge(const DigitalSignal* signal, uint32_t edge_num) {
    furi_assert(signal);
    furi_assert(edge_num < signal->edge_cnt);

    return signal->edge_timings[edge_num];
}

void digital_signal_prepare_arr(DigitalSignal* signal) {
    furi_assert(signal);

    DigitalSignalInternals* internals = signal->internals;

    /* set up signal polarities */
    if(internals->gpio) {
        uint32_t bit_set = internals->gpio->pin;
        uint32_t bit_reset = internals->gpio->pin << 16;

#ifdef DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN
        bit_set |= DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN.pin;
        bit_reset |= DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN.pin << 16;
#endif

        if(signal->start_level) {
            internals->gpio_buff[0] = bit_set;
            internals->gpio_buff[1] = bit_reset;
        } else {
            internals->gpio_buff[0] = bit_reset;
            internals->gpio_buff[1] = bit_set;
        }
    }

    /* set up edge timings */
    internals->reload_reg_entries = 0;

    for(size_t pos = 0; pos < signal->edge_cnt; pos++) {
        uint32_t pulse_duration = signal->edge_timings[pos] + internals->reload_reg_remainder;
        if(pulse_duration < 10 || pulse_duration > 10000000) {
            FURI_LOG_D(
                TAG,
                "[prepare] pulse_duration out of range: %lu = %lu * %llu",
                pulse_duration,
                signal->edge_timings[pos],
                internals->factor);
            pulse_duration = 100;
        }
        uint32_t pulse_ticks = (pulse_duration + T_TIM_DIV2) / T_TIM;
        internals->reload_reg_remainder = pulse_duration - (pulse_ticks * T_TIM);

        if(pulse_ticks > 1) {
            signal->reload_reg_buff[internals->reload_reg_entries++] = pulse_ticks - 1;
        }
    }
}
