#include "digital_signal.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <math.h>

#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_tim.h>

struct DigitalSequence {
    uint8_t signals_size;
    bool bake;
    uint32_t sequence_used;
    uint32_t sequence_size;
    DigitalSignal** signals;
    bool* signals_prolonged;
    uint8_t* sequence;
    const GpioPin* gpio;
    uint32_t send_time;
    bool send_time_active;
};

struct DigitalSignalInternals {
    uint32_t reload_reg_entries;
    uint32_t reload_reg_remainder;
    uint32_t gpio_buff[2];
    const GpioPin* gpio;
    LL_DMA_InitTypeDef dma_config_gpio;
    LL_DMA_InitTypeDef dma_config_timer;
};

#define TAG "DigitalSignal"

#define F_TIM (64000000.0)
#define T_TIM 1562 /* 15.625 ns *100 */
#define T_TIM_DIV2 781 /* 15.625 ns / 2 *100 */

DigitalSignal* digital_signal_alloc(uint32_t max_edges_cnt) {
    DigitalSignal* signal = malloc(sizeof(DigitalSignal));
    signal->start_level = true;
    signal->edges_max_cnt = max_edges_cnt;
    signal->edge_timings = malloc(signal->edges_max_cnt * sizeof(uint32_t));
    signal->edge_cnt = 0;
    signal->reload_reg_buff = malloc(signal->edges_max_cnt * sizeof(uint32_t));

    signal->internals = malloc(sizeof(DigitalSignalInternals));
    signal->internals->reload_reg_entries = 0;
    signal->internals->reload_reg_remainder = 0;
    signal->internals->dma_config_gpio.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    signal->internals->dma_config_gpio.Mode = LL_DMA_MODE_CIRCULAR;
    signal->internals->dma_config_gpio.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    signal->internals->dma_config_gpio.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    signal->internals->dma_config_gpio.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    signal->internals->dma_config_gpio.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    signal->internals->dma_config_gpio.NbData = 2;
    signal->internals->dma_config_gpio.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    signal->internals->dma_config_gpio.Priority = LL_DMA_PRIORITY_VERYHIGH;

    signal->internals->dma_config_timer.PeriphOrM2MSrcAddress = (uint32_t) & (TIM2->ARR);
    signal->internals->dma_config_timer.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    signal->internals->dma_config_timer.Mode = LL_DMA_MODE_NORMAL;
    signal->internals->dma_config_timer.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    signal->internals->dma_config_timer.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    signal->internals->dma_config_timer.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    signal->internals->dma_config_timer.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    signal->internals->dma_config_timer.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    signal->internals->dma_config_timer.Priority = LL_DMA_PRIORITY_HIGH;

    return signal;
}

void digital_signal_free(DigitalSignal* signal) {
    furi_assert(signal);

    if(!signal) {
        return;
    }

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

uint32_t digital_signal_get_edge(DigitalSignal* signal, uint32_t edge_num) {
    furi_assert(signal);
    furi_assert(edge_num < signal->edge_cnt);

    return signal->edge_timings[edge_num];
}

void digital_signal_prepare_arr(DigitalSignal* signal) {
    furi_assert(signal);

    /* set up signal polarities */
    if(signal->internals->gpio) {
        uint32_t bit_set = signal->internals->gpio->pin;
        uint32_t bit_reset = signal->internals->gpio->pin << 16;

        if(signal->start_level) {
            signal->internals->gpio_buff[0] = bit_set;
            signal->internals->gpio_buff[1] = bit_reset;
        } else {
            signal->internals->gpio_buff[0] = bit_reset;
            signal->internals->gpio_buff[1] = bit_set;
        }
    }

    /* set up edge timings */
    signal->internals->reload_reg_entries = 0;

    for(size_t pos = 0; pos < signal->edge_cnt; pos++) {
        uint32_t pulse_duration =
            signal->edge_timings[pos] + signal->internals->reload_reg_remainder;
        uint32_t pulse_ticks = (pulse_duration + T_TIM_DIV2) / T_TIM;
        signal->internals->reload_reg_remainder = pulse_duration - (pulse_ticks * T_TIM);

        if(pulse_ticks > 1) {
            signal->reload_reg_buff[signal->internals->reload_reg_entries++] = pulse_ticks - 1;
        }
    }
}

static void digital_signal_stop_dma() {
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ClearFlag_TC2(DMA1);
}

static void digital_signal_stop_timer() {
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetCounter(TIM2, 0);
}

static bool digital_signal_setup_dma(DigitalSignal* signal) {
    furi_assert(signal);

    if(!signal->internals->reload_reg_entries) {
        return false;
    }

    signal->internals->dma_config_gpio.MemoryOrM2MDstAddress =
        (uint32_t)signal->internals->gpio_buff;
    signal->internals->dma_config_gpio.PeriphOrM2MSrcAddress =
        (uint32_t) & (signal->internals->gpio->port->BSRR);
    signal->internals->dma_config_timer.MemoryOrM2MDstAddress = (uint32_t)signal->reload_reg_buff;
    signal->internals->dma_config_timer.NbData = signal->internals->reload_reg_entries;

    /* set up DMA channel 1 and 2 for GPIO and timer copy operations */
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &signal->internals->dma_config_gpio);
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_2, &signal->internals->dma_config_timer);

    /* enable both DMA channels */
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

    return true;
}

static void digital_signal_setup_timer() {
    digital_signal_stop_timer();

    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    LL_TIM_SetAutoReload(TIM2, 10);
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableUpdateEvent(TIM2);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);
}

static void digital_signal_start_timer() {
    LL_TIM_GenerateEvent_UPDATE(TIM2);
    LL_TIM_EnableCounter(TIM2);
}

void digital_signal_send(DigitalSignal* signal, const GpioPin* gpio) {
    furi_assert(signal);

    if(!signal->edge_cnt) {
        return;
    }

    /* Configure gpio as output */
    signal->internals->gpio = gpio;
    furi_hal_gpio_init(
        signal->internals->gpio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    /* single signal, add a temporary, terminating edge at the end */
    signal->edge_timings[signal->edge_cnt++] = 10;
    digital_signal_prepare_arr(signal);

    digital_signal_setup_dma(signal);
    digital_signal_setup_timer();
    digital_signal_start_timer();

    while(!LL_DMA_IsActiveFlag_TC2(DMA1)) {
    }

    digital_signal_stop_timer();
    digital_signal_stop_dma();

    signal->edge_cnt--;
}

void digital_sequence_alloc_signals(DigitalSequence* sequence, uint32_t size) {
    sequence->signals_size = size;
    sequence->signals = malloc(sequence->signals_size * sizeof(DigitalSignal*));
    sequence->signals_prolonged = malloc(sequence->signals_size * sizeof(bool));
}

void digital_sequence_alloc_sequence(DigitalSequence* sequence, uint32_t size) {
    sequence->sequence_used = 0;
    sequence->sequence_size = size;
    sequence->sequence = malloc(sequence->sequence_size);
    sequence->send_time = 0;
    sequence->send_time_active = false;
}

DigitalSequence* digital_sequence_alloc(uint32_t size, const GpioPin* gpio) {
    furi_assert(gpio);

    DigitalSequence* sequence = malloc(sizeof(DigitalSequence));

    sequence->gpio = gpio;
    sequence->bake = false;

    digital_sequence_alloc_signals(sequence, 32);
    digital_sequence_alloc_sequence(sequence, size);

    return sequence;
}

void digital_sequence_free(DigitalSequence* sequence) {
    furi_assert(sequence);

    if(!sequence) {
        return;
    }

    free(sequence->signals);
    free(sequence->sequence);
    free(sequence->signals_prolonged);
    free(sequence);
}

void digital_sequence_set_signal(
    DigitalSequence* sequence,
    uint8_t signal_index,
    DigitalSignal* signal) {
    furi_assert(sequence);
    furi_assert(signal);
    furi_assert(signal_index < sequence->signals_size);

    sequence->signals[signal_index] = signal;
    signal->internals->gpio = sequence->gpio;
    signal->internals->reload_reg_remainder = 0;

    digital_signal_prepare_arr(signal);
}

void digital_sequence_set_sendtime(DigitalSequence* sequence, uint32_t send_time) {
    furi_assert(sequence);

    sequence->send_time = send_time;
    sequence->send_time_active = true;
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

static void digital_signal_update_dma(DigitalSignal* signal) {
    /* keep them prepared in registers so there is less delay when writing */
    register bool restart_needed = false;
    register volatile uint16_t len = signal->internals->reload_reg_entries;
    register volatile uint32_t addr = (uint32_t)signal->reload_reg_buff;

    /* first make sure it will still count down, else we will risk waiting infinitely */
    const uint32_t wait_ms = 10;
    const uint32_t wait_ticks = wait_ms * 1000 * furi_hal_cortex_instructions_per_microsecond();
    uint16_t prev_remain = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2);
    uint32_t prev_timer = DWT->CYCCNT;

    while(prev_remain == LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2)) {
        if(DWT->CYCCNT - prev_timer > wait_ticks) {
            restart_needed = true;
            break;
        }
    }

    if(!restart_needed) {
        /* if transfer was already active, wait till DMA is done and the last timer ticks are running */
        while(LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2)) {
        }
    }

    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, len);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, addr);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

    if(restart_needed) {
        LL_TIM_GenerateEvent_UPDATE(TIM2);
    }
}

static bool digital_sequence_send_signal(DigitalSequence* sequence, DigitalSignal* signal) {
    /* the first iteration has to set up the whole machinery */
    if(!LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_1)) {
        if(!digital_signal_setup_dma(signal)) {
            FURI_LOG_D(TAG, "digital_sequence_send_signal: Signal has no entries, aborting");
            return false;
        }
        digital_signal_setup_timer();

        /* if the send time is specified, wait till the core timer passed beyond that time */
        if(sequence->send_time_active) {
            sequence->send_time_active = false;
            while(sequence->send_time - DWT->CYCCNT < 0x80000000) {
            }
        }
        digital_signal_start_timer();
    } else {
        /* configure next polarities and timings */
        digital_signal_update_dma(signal);
    }

    return true;
}

DigitalSignal* digital_sequence_bake(DigitalSequence* sequence) {
    furi_assert(sequence);

    uint32_t edges = 0;

    for(uint32_t pos = 0; pos < sequence->sequence_used; pos++) {
        uint8_t signal_index = sequence->sequence[pos];
        DigitalSignal* sig = sequence->signals[signal_index];

        edges += sig->edge_cnt;
    }

    DigitalSignal* ret = digital_signal_alloc(edges);

    for(uint32_t pos = 0; pos < sequence->sequence_used; pos++) {
        uint8_t signal_index = sequence->sequence[pos];
        DigitalSignal* sig = sequence->signals[signal_index];

        digital_signal_append(ret, sig);
    }

    return ret;
}

bool digital_sequence_send(DigitalSequence* sequence) {
    furi_assert(sequence);

    furi_hal_gpio_init(sequence->gpio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    if(sequence->bake) {
        DigitalSignal* sig = digital_sequence_bake(sequence);

        digital_signal_send(sig, sequence->gpio);
        digital_signal_free(sig);
        return true;
    }

    int32_t remainder = 0;
    FURI_CRITICAL_ENTER();

    for(uint32_t pos = 0; pos < sequence->sequence_used; pos++) {
        uint8_t signal_index = sequence->sequence[pos];
        DigitalSignal* sig = sequence->signals[signal_index];

        if(!sig) {
            FURI_LOG_D(
                TAG,
                "digital_sequence_send: Signal at index %u, used at pos %lu is NULL, aborting",
                signal_index,
                pos);
            break;
        }

        /* when we are too late more than half a tick, make the first edge temporarily longer */
        bool needs_prolongation = false;

        if(remainder >= T_TIM_DIV2) {
            remainder -= T_TIM;
            needs_prolongation = true;
        }

        /* update the total remainder */
        remainder += sig->internals->reload_reg_remainder;

        /* do we need to update the prolongation? */
        if(needs_prolongation != sequence->signals_prolonged[signal_index]) {
            if(needs_prolongation) {
                sig->edge_timings[0]++;
            } else {
                sig->edge_timings[0]--;
            }
            sequence->signals_prolonged[signal_index] = needs_prolongation;
        }

        bool success = digital_sequence_send_signal(sequence, sig);

        if(!success) {
            break;
        }
    }

    /* wait until last dma transaction was finished */
    while(LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2)) {
    }

    digital_signal_stop_timer();
    digital_signal_stop_dma();
    FURI_CRITICAL_EXIT();

    /* undo previously prolonged edges */
    for(uint32_t pos = 0; pos < sequence->signals_size; pos++) {
        DigitalSignal* sig = sequence->signals[pos];

        if(sig && sequence->signals_prolonged[pos]) {
            sig->edge_timings[0]--;
            sequence->signals_prolonged[pos] = false;
        }
    }

    return true;
}

void digital_sequence_clear(DigitalSequence* sequence) {
    furi_assert(sequence);

    sequence->sequence_used = 0;
}
