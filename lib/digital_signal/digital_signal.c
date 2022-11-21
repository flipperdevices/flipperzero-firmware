#include "digital_signal.h"

#include <furi.h>
#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_tim.h>
#include <math.h>

#define TAG "[DigitalSignal]"

#pragma GCC optimize("O3,unroll-loops,Ofast")

#define F_TIM (64000000.0)
#define T_TIM 1562 //15.625 ns *100
#define T_TIM_DIV2 781 //15.625 ns / 2 *100

DigitalSignal* digital_signal_alloc(uint32_t max_edges_cnt) {
    DigitalSignal* signal = malloc(sizeof(DigitalSignal));
    signal->prepared = false;
    signal->start_level = true;
    signal->edges_max_cnt = max_edges_cnt;
    signal->edge_timings = malloc(signal->edges_max_cnt * sizeof(uint32_t));
    signal->edge_cnt = 0;
    signal->reload_reg_buff = malloc(signal->edges_max_cnt * sizeof(uint32_t));
    signal->reload_reg_entries = 0;
    signal->reload_reg_remainder = 0;

    return signal;
}

void digital_signal_free(DigitalSignal* signal) {
    furi_assert(signal);

    free(signal->edge_timings);
    free(signal->reload_reg_buff);
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

    signal_a->prepared = false;

    return true;
}

bool digital_signal_get_start_level(DigitalSignal* signal) {
    furi_assert(signal);

    return signal->start_level;
}

uint32_t digital_signal_get_edges_cnt(DigitalSignal* signal) {
    furi_assert(signal);

    return signal->edge_cnt;
}

void digital_signal_add(DigitalSignal* signal, uint32_t ticks) {
    furi_assert(signal);
    furi_assert(signal->edge_cnt < signal->edges_max_cnt);

    signal->edge_timings[signal->edge_cnt++] = ticks;
}

uint32_t digital_signal_get_edge(DigitalSignal* signal, uint32_t edge_num) {
    furi_assert(signal);
    furi_assert(edge_num < signal->edge_cnt);

    return signal->edge_timings[edge_num];
}

void digital_signal_prepare(DigitalSignal* signal) {
    furi_assert(signal);
    
    /* set up signal polarities */
    uint32_t bit_set = signal->gpio->pin;
    uint32_t bit_reset = signal->gpio->pin << 16;

    if(signal->start_level) {
        signal->gpio_buff[0] = bit_set;
        signal->gpio_buff[1] = bit_reset;
    } else {
        signal->gpio_buff[0] = bit_reset;
        signal->gpio_buff[1] = bit_set;
    }

    /* set up edge timings */
    signal->reload_reg_entries = 0;

    for(size_t pos = 0; pos < signal->edge_cnt; pos++) {
        uint32_t pulse_duration = signal->edge_timings[pos] + signal->reload_reg_remainder;
        uint32_t pulse_ticks = (pulse_duration + T_TIM_DIV2) / T_TIM;
        signal->reload_reg_remainder = pulse_duration - (pulse_ticks * T_TIM);

        if(pulse_ticks > 1) {
            signal->reload_reg_buff[signal->reload_reg_entries++] = pulse_ticks - 1;
        }
    }
}


void digital_signal_stop_dma() {
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ClearFlag_TC2(DMA1);
}

void digital_signal_stop_timer() {
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetCounter(TIM2, 0);
}

bool digital_signal_setup_dma(DigitalSignal* signal) {
    furi_assert(signal);

    if(!signal->reload_reg_entries) {
        return false;
    }

    LL_DMA_InitTypeDef dma_config_gpio = {};
    LL_DMA_InitTypeDef dma_config_timer = {};

    dma_config_gpio.MemoryOrM2MDstAddress = (uint32_t) signal->gpio_buff;
    dma_config_gpio.PeriphOrM2MSrcAddress = (uint32_t) &(signal->gpio->port->BSRR);
    dma_config_gpio.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config_gpio.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config_gpio.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config_gpio.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config_gpio.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config_gpio.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config_gpio.NbData = 2;
    dma_config_gpio.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    dma_config_gpio.Priority = LL_DMA_PRIORITY_VERYHIGH;

    // Init timer arr register buffer and DMA channel
    dma_config_timer.MemoryOrM2MDstAddress = (uint32_t)signal->reload_reg_buff;
    dma_config_timer.PeriphOrM2MSrcAddress = (uint32_t) &(TIM2->ARR);
    dma_config_timer.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config_timer.Mode = LL_DMA_MODE_NORMAL;
    dma_config_timer.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config_timer.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config_timer.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config_timer.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config_timer.NbData = signal->reload_reg_entries;
    dma_config_timer.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    dma_config_timer.Priority = LL_DMA_PRIORITY_HIGH;

    digital_signal_stop_dma();

    /* set up DMA channel 1 and 2 for GPIO and timer copy operations */
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &dma_config_gpio);
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_2, &dma_config_timer);

    /* enable both DMA channels */
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

    return true;
}


bool digital_signal_update_dma(DigitalSignal* signal) {
    furi_assert(signal);

    if(!signal->reload_reg_entries) {
        return false;
    }

    digital_signal_stop_dma();

    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)signal->gpio_buff);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)signal->reload_reg_buff);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 2);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, signal->reload_reg_entries);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

    return true;
}

void digital_signal_setup_timer() {
    
    digital_signal_stop_timer();

    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    LL_TIM_SetAutoReload(TIM2, 10);
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableUpdateEvent(TIM2);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);
}

void digital_signal_start_timer() {
    LL_TIM_GenerateEvent_UPDATE(TIM2);
    LL_TIM_EnableCounter(TIM2);
}

void digital_signal_send(DigitalSignal* signal, const GpioPin* gpio) {
    furi_assert(signal);

    /* if selected GPIO changed, force reconfiguration of buffers */
    if(gpio && (signal->gpio != gpio)) {
        signal->gpio = gpio;
    }

    /* Configure gpio as output */
    furi_hal_gpio_init(signal->gpio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    digital_signal_prepare(signal);

    digital_signal_setup_dma(signal);
    digital_signal_setup_timer();
    digital_signal_start_timer();

    while(!LL_DMA_IsActiveFlag_TC2(DMA1)) {
    }

    digital_signal_stop_timer();
    digital_signal_stop_dma();
}


DigitalSequence* digital_sequence_alloc(uint32_t size, const GpioPin* gpio) {

    DigitalSequence* sequence = malloc(sizeof(DigitalSequence));

    sequence->gpio = gpio;
    sequence->signals_size = 32;
    sequence->signals = malloc(sequence->signals_size * sizeof(DigitalSignal*));

    sequence->sequence_used = 0;
    sequence->sequence_size = size;
    sequence->sequence = malloc(sequence->sequence_size);

    return sequence;
}

void digital_sequence_free(DigitalSequence* sequence) {
    furi_assert(sequence);

    free(sequence->signals);
    free(sequence->sequence);
    free(sequence);
}

void digital_sequence_set_signal(DigitalSequence* sequence, uint8_t signal_index, DigitalSignal* signal) {
    furi_assert(sequence);
    furi_assert(signal);
    furi_assert(signal_index < sequence->signals_size);

    sequence->signals[signal_index] = signal;

    /* all signals will use the sequence's GPIO */
    signal->gpio = sequence->gpio;
    signal->prepared = false;

    digital_signal_prepare(signal);
}

void digital_sequence_add(DigitalSequence* sequence, uint8_t signal_index) {
    furi_assert(sequence);
    furi_assert(signal_index < sequence->signals_size);

    if(sequence->sequence_used >= sequence->sequence_size) {
        sequence->sequence_size += 256;
        sequence->sequence = realloc(sequence->sequence, sequence->sequence_size);
    }

    sequence->sequence[sequence->sequence_used++] = signal_index;
}

bool digital_sequence_send_signal(DigitalSignal* signal) {
    furi_assert(signal);
    
    /* the first iteration has to set up the whole machinery */
    if(!LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_1)) {
        furi_hal_gpio_init(signal->gpio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

        if(!digital_signal_setup_dma(signal)) {
            FURI_LOG_D(TAG, "Signal has no entries, aborting");
            return false;
        }
        digital_signal_setup_timer();
        digital_signal_start_timer();
    } else {
        /* transfer was already active, wait till DMA is done and the last timer ticks are running */
        while(!LL_DMA_IsActiveFlag_TC2(DMA1)) {
        }

        /* configure next polarities and timings */
        if(!digital_signal_update_dma(signal)) {
            FURI_LOG_D(TAG, "Signal has no entries, aborting");
            return false;
        }
    }

    return true;
}

bool digital_sequence_send(DigitalSequence* sequence) {
    furi_assert(sequence);

    uint32_t remainder = 0;

    for(uint32_t pos = 0; pos < sequence->sequence_used; pos++) {
        DigitalSignal *sig = sequence->signals[sequence->sequence[pos]];

        /* take over previous remainder */
        sig->reload_reg_remainder = remainder;
        digital_signal_prepare(sig);

        if(!digital_sequence_send_signal(sig)) {
            digital_signal_stop_timer();
            digital_signal_stop_dma();
            return false;
        }
    }

    while(!LL_DMA_IsActiveFlag_TC2(DMA1)) {
    }

    digital_signal_stop_timer();
    digital_signal_stop_dma();

    return true;
}

void digital_sequence_clear(DigitalSequence* sequence) {
    furi_assert(sequence);

    sequence->sequence_used = 0;
}
