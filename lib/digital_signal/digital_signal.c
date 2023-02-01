#include "digital_signal.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <math.h>

#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_tim.h>


struct ReloadBuffers {
    uint32_t** buffers; /* pointers to the shadow buffers, either one or two. NULL if none */
    uint32_t count; /* number of allocated buffers, 0, 1 or 2 */
    uint32_t size; /* maximum entry count of a single buffer */
    uint32_t current; /* current buffer index, the other one is most likely being used */
    uint32_t entries; /* entries in the current buffer */
};

struct DigitalSequence {
    uint8_t signals_size;
    bool bake;
    uint32_t sequence_used;
    uint32_t sequence_size;
    DigitalSignal** signals;
    uint8_t* sequence;
    const GpioPin* gpio;
    uint32_t send_time;
    bool send_time_active;
    struct ReloadBuffers* reload;
};

struct DigitalSignalInternals {
    uint32_t reload_reg_entries;
    uint32_t reload_reg_remainder;
    uint32_t gpio_buff[2];
    const GpioPin* gpio;
    LL_DMA_InitTypeDef dma_config_gpio;
    LL_DMA_InitTypeDef dma_config_timer;
    struct ReloadBuffers* reload;
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
    DigitalSignalInternals* internals = signal->internals;
    internals->reload = NULL;
    internals->reload_reg_entries = 0;
    internals->reload_reg_remainder = 0;

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

    if(!signal) {
        return;
    }

    free(signal->edge_timings);
    free(signal->reload_reg_buff);
    if(signal->internals->reload) {
        if(signal->internals->reload->buffers) {
            free(signal->internals->reload->buffers);
        }
        free(signal->internals->reload);
    }
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

    DigitalSignalInternals* internals = signal->internals;

    /* set up signal polarities */
    if(internals->gpio) {
        uint32_t bit_set = internals->gpio->pin;
        uint32_t bit_reset = internals->gpio->pin << 16;

#ifdef DEBUG_OUTPUT
        bit_set |= gpio_ext_pb3.pin;
        bit_reset |= gpio_ext_pb3.pin << 16;
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
        uint32_t pulse_ticks = (pulse_duration + T_TIM_DIV2) / T_TIM;
        internals->reload_reg_remainder = pulse_duration - (pulse_ticks * T_TIM);

        if(pulse_ticks > 1) {
            signal->reload_reg_buff[internals->reload_reg_entries++] = pulse_ticks - 1;
        }
    }

    /* in case there are no shadow buffers defined, allocate and use the precalced data */
    if(!internals->reload || !internals->reload->count) {
        if(internals->reload) {
            free(internals->reload);
        }
        internals->reload = malloc(sizeof(struct ReloadBuffers));
        internals->reload->count = 1;
        internals->reload->size = signal->edges_max_cnt;
        internals->reload->buffers = malloc(sizeof(uint32_t*));
        internals->reload->buffers[0] = malloc(internals->reload->size * sizeof(uint32_t));
        memcpy(
            internals->reload->buffers[0],
            signal->reload_reg_buff,
            internals->reload_reg_entries * sizeof(uint32_t));
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
    LL_TIM_DisableUpdateEvent(TIM2);
    LL_TIM_DisableDMAReq_UPDATE(TIM2);
}

static void digital_signal_setup_timer() {
    digital_signal_stop_timer();

    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    LL_TIM_SetAutoReload(TIM2, 0xFFFFFFFF);
    LL_TIM_SetCounter(TIM2, 0);
}

static void digital_signal_start_timer() {
    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableUpdateEvent(TIM2);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);
    LL_TIM_GenerateEvent_UPDATE(TIM2);
}

static bool digital_signal_setup_dma(DigitalSignal* signal) {
    furi_assert(signal);
    DigitalSignalInternals* internals = signal->internals;

    uint32_t buffer_entries = internals->reload->entries;
    if(!buffer_entries || !internals->reload || !internals->reload->buffers) {
        return false;
    }
    digital_signal_stop_dma();

    internals->dma_config_gpio.MemoryOrM2MDstAddress = (uint32_t)internals->gpio_buff;
    internals->dma_config_gpio.PeriphOrM2MSrcAddress = (uint32_t) & (internals->gpio->port->BSRR);
    internals->dma_config_timer.MemoryOrM2MDstAddress =
        (uint32_t)internals->reload->buffers[internals->reload->current];
    internals->dma_config_timer.NbData = buffer_entries;

    /* set up DMA channel 1 and 2 for GPIO and timer copy operations */
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &internals->dma_config_gpio);
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_2, &internals->dma_config_timer);

    /* enable both DMA channels */
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

    /* buffer is used now by DMA, skip to next */
    internals->reload->current = (internals->reload->current + 1) % internals->reload->count;

    return true;
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

    sequence->reload = malloc(sizeof(struct ReloadBuffers));
    sequence->reload->count = 2;
    sequence->reload->size = 512;
    sequence->reload->buffers = malloc(sizeof(uint32_t*));
    sequence->reload->buffers[0] = malloc(sequence->reload->size * sizeof(uint32_t));
    sequence->reload->buffers[1] = malloc(sequence->reload->size * sizeof(uint32_t));

    digital_sequence_alloc_signals(sequence, 32);
    digital_sequence_alloc_sequence(sequence, size);

    return sequence;
}

void digital_sequence_free(DigitalSequence* sequence) {
    furi_assert(sequence);

    if(!sequence) {
        return;
    }

    /* de-assign the shared reload buffer */
    for(int pos = 0; pos < sequence->signals_size; pos++) {
        if(sequence->signals[pos]) {
            sequence->signals[pos]->internals->reload = NULL;
        }
    }

    free(sequence->signals);
    free(sequence->sequence);
    free(sequence->reload->buffers);
    free(sequence->reload);
    free(sequence);
}

void digital_sequence_set_signal(
    DigitalSequence* sequence,
    uint8_t signal_index,
    DigitalSignal* signal) {
    furi_assert(sequence);
    furi_assert(signal);
    furi_assert(signal_index < sequence->signals_size);

    /* if there is already a signal, unassign the shared reload buffer */
    if(sequence->signals[signal_index]) {
        sequence->signals[signal_index]->internals->reload = NULL;
    }

    sequence->signals[signal_index] = signal;
    signal->internals->gpio = sequence->gpio;
    signal->internals->reload_reg_remainder = 0;

    /* free the original reload buffer */
    if(signal->internals->reload) {
        if(signal->internals->reload->buffers) {
            for(uint32_t pos = 0; pos < signal->internals->reload->count; pos++) {
                free(signal->internals->reload->buffers[pos]);
            }
            free(signal->internals->reload->buffers);
        }
        free(signal->internals->reload);
    }

    /* assign the sequence's shared reload buffer */
    signal->internals->reload = sequence->reload;

    /* ensure it is big enough and reallocate if not */
    if(sequence->reload->size < signal->edges_max_cnt) {
        free(sequence->reload->buffers);

        sequence->reload->size = signal->edges_max_cnt;
        sequence->reload->buffers[0] = malloc(sequence->reload->size * sizeof(uint32_t));
        sequence->reload->buffers[1] = malloc(sequence->reload->size * sizeof(uint32_t));
    }

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
    struct ReloadBuffers* reload = signal->internals->reload;
    /* keep them prepared in registers so there is less delay when writing */
    register bool restart_needed = false;
    register volatile uint16_t len = reload->entries;
    register volatile uint32_t addr = (uint32_t)reload->buffers[reload->current];

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
    } else {
        FURI_LOG_D(TAG, "digital_sequence_send_signal: DMA hung, restart needed");
    }

    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, len);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, addr);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

    if(restart_needed) {
        LL_TIM_GenerateEvent_UPDATE(TIM2);
    }

    reload->current = (reload->current + 1) % reload->count;
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

    struct ReloadBuffers* reload = sequence->reload;

    furi_hal_gpio_init(sequence->gpio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
#ifdef DEBUG_OUTPUT
    furi_hal_gpio_init(&gpio_ext_pb3, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
#endif

    if(sequence->bake) {
        DigitalSignal* sig = digital_sequence_bake(sequence);

        digital_signal_send(sig, sequence->gpio);
        digital_signal_free(sig);
        return true;
    }

    int32_t remainder = 0;
    FURI_CRITICAL_ENTER();

    bool traded_first = false;

    for(uint32_t pos = 0; pos < sequence->sequence_used; pos++) {
        uint8_t signal_index = sequence->sequence[pos];
        DigitalSignal* sig = sequence->signals[signal_index];
        DigitalSignal* sig_next = NULL;

        if(pos + 1 < sequence->sequence_used) {
            sig_next = sequence->signals[sequence->sequence[pos + 1]];
        }

        if(!sig) {
            FURI_LOG_D(
                TAG,
                "digital_sequence_send: Signal at index %u, used at pos %lu is NULL, aborting",
                signal_index,
                pos);
            break;
        }

        /* if the first edge is handled by prolonging the last pulse of the previous signal, skip it here */
        reload->entries = sig->edge_cnt - (traded_first ? 1 : 0);

        memcpy(
            reload->buffers[reload->current],
            &sig->reload_reg_buff[traded_first ? 1 : 0],
            reload->entries * sizeof(uint32_t));
        traded_first = false;

        /* when we are too late more than half a tick, make the first edge temporarily longer */
        if(remainder >= T_TIM_DIV2) {
            remainder -= T_TIM;
            reload->buffers[reload->current][0] += 1;
        }

        /* update the total remainder */
        remainder += sig->internals->reload_reg_remainder;

        /* when a signal ends with the same level as the next signal begins, let the fist signal generate the whole pulse */
        if(sig_next) {
            /* beware, we do not want the level after the last edge, but the last level before that edge */
            bool end_level = sig->start_level ^ ((sig->edge_cnt % 2) == 0);

            /* take from the next, add it to the first */
            if(end_level == sig_next->start_level) {
                /* add the traded prolongation to the last pulse */
                reload->buffers[reload->current][reload->entries - 1] +=
                    sig_next->reload_reg_buff[0];
                traded_first = true;
            }
        }

        /* transmit */
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

    return true;
}

void digital_sequence_clear(DigitalSequence* sequence) {
    furi_assert(sequence);

    sequence->sequence_used = 0;
}
