#include "digital_sequence.h"
#include "digital_signal_i.h"

#include <furi.h>
#include <furi_hal_bus.h>

#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_tim.h>

#define TAG "DigitalSequence"

/* end marker in DMA ringbuffer, will get written into timer register at the end */
#define SEQ_TIMER_MAX 0xFFFFFFFF

/* time to wait in loops before returning */
#define SEQ_LOCK_WAIT_MS 10UL
#define SEQ_LOCK_WAIT_TICKS (SEQ_LOCK_WAIT_MS * 1000 * 64)

/* maximum entry count of the sequence dma ring buffer */
#define RINGBUFFER_SIZE 128

/* maximum number of DigitalSignals in a sequence */
#define SEQUENCE_SIGNALS_SIZE 32
/*
 * if sequence size runs out from the initial value passed to digital_sequence_alloc
 * the size will be increased by this amount and reallocated
 */
#define SEQUENCE_SIZE_REALLOCATE_INCREMENT 256

/* must be on bank B */
// For debugging purposes use `--extra-define=DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN=gpio_ext_pb3` fbt option

typedef struct {
    uint32_t* buffer; /* DMA ringbuffer */
    uint32_t write_pos; /* current buffer write index */
    uint32_t read_pos; /* current buffer read index */
} ReloadBuffer;

struct DigitalSequence {
    uint8_t signals_size;
    uint32_t sequence_used;
    uint32_t sequence_size;
    DigitalSignal** signals;
    uint8_t* sequence;
    const GpioPin* gpio;
    LL_DMA_InitTypeDef dma_config_gpio;
    LL_DMA_InitTypeDef dma_config_timer;
    bool dma_active;

    uint32_t gpio_buff[2];
    ReloadBuffer dma_buffer;

    uint32_t send_time;
    bool send_time_active;
};

DigitalSequence* digital_sequence_alloc(uint32_t size, const GpioPin* gpio) {
    furi_assert(gpio);

    DigitalSequence* sequence = malloc(sizeof(DigitalSequence));

    sequence->gpio = gpio;

    sequence->signals_size = SEQUENCE_SIGNALS_SIZE;
    sequence->signals = malloc(sequence->signals_size * sizeof(DigitalSignal*));

    sequence->sequence_size = size;
    sequence->sequence = malloc(sequence->sequence_size);

    sequence->dma_buffer.buffer = malloc(RINGBUFFER_SIZE * sizeof(uint32_t));

    sequence->dma_config_gpio.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    sequence->dma_config_gpio.Mode = LL_DMA_MODE_CIRCULAR;
    sequence->dma_config_gpio.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    sequence->dma_config_gpio.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    sequence->dma_config_gpio.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    sequence->dma_config_gpio.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    sequence->dma_config_gpio.NbData = 2;
    sequence->dma_config_gpio.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    sequence->dma_config_gpio.Priority = LL_DMA_PRIORITY_VERYHIGH;

    sequence->dma_config_timer.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    sequence->dma_config_timer.Mode = LL_DMA_MODE_CIRCULAR;
    sequence->dma_config_timer.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    sequence->dma_config_timer.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    sequence->dma_config_timer.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    sequence->dma_config_timer.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    sequence->dma_config_timer.PeriphOrM2MSrcAddress = (uint32_t) & (TIM2->ARR);
    sequence->dma_config_timer.MemoryOrM2MDstAddress = (uint32_t)sequence->dma_buffer.buffer;
    sequence->dma_config_timer.NbData = RINGBUFFER_SIZE;
    sequence->dma_config_timer.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    sequence->dma_config_timer.Priority = LL_DMA_PRIORITY_HIGH;

    return sequence;
}

void digital_sequence_free(DigitalSequence* sequence) {
    furi_assert(sequence);

    free(sequence->signals);
    free(sequence->sequence);
    free(sequence->dma_buffer.buffer);
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
        sequence->sequence_size += SEQUENCE_SIZE_REALLOCATE_INCREMENT;
        sequence->sequence = realloc(sequence->sequence, sequence->sequence_size); //-V701
        furi_assert(sequence->sequence);
    }

    sequence->sequence[sequence->sequence_used++] = signal_index;
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

    furi_hal_bus_disable(FuriHalBusTIM2);
}

static void digital_sequence_setup_timer() {
    furi_hal_bus_enable(FuriHalBusTIM2);

    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    LL_TIM_SetAutoReload(TIM2, SEQ_TIMER_MAX);
    LL_TIM_SetCounter(TIM2, 0);
}

static void digital_sequence_start_timer() {
    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableUpdateEvent(TIM2);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);
    LL_TIM_GenerateEvent_UPDATE(TIM2);
}

static inline void digital_sequence_setup_gpio(DigitalSequence* sequence, const DigitalSignal* first_signal) {
    uint32_t bit_set = sequence->gpio->pin;
    uint32_t bit_reset = sequence->gpio->pin << 16;

#ifdef DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN
    bit_set |= DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN.pin;
    bit_reset |= DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN.pin << 16;
#endif

    if(first_signal->start_level) {
        sequence->gpio_buff[0] = bit_set;
        sequence->gpio_buff[1] = bit_reset;
    } else {
        sequence->gpio_buff[0] = bit_reset;
        sequence->gpio_buff[1] = bit_set;
    }
}

static bool digital_sequence_setup_dma(DigitalSequence* sequence) {
    furi_assert(sequence);

    digital_signal_stop_dma();

    sequence->dma_config_gpio.MemoryOrM2MDstAddress = (uint32_t)sequence->gpio_buff;
    sequence->dma_config_gpio.PeriphOrM2MSrcAddress = (uint32_t)&sequence->gpio->port->BSRR;

    /* set up DMA channel 1 and 2 for GPIO and timer copy operations */
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &sequence->dma_config_gpio);
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_2, &sequence->dma_config_timer);

    /* enable both DMA channels */
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

    return true;
}

static void digital_sequence_finish(DigitalSequence* sequence) {
    if(sequence->dma_active) {
        uint32_t prev_timer = DWT->CYCCNT;
        do {
            /* we are finished, when the DMA transferred the SEQ_TIMER_MAX marker */
            if(TIM2->ARR == SEQ_TIMER_MAX) {
                break;
            }
            if(DWT->CYCCNT - prev_timer > SEQ_LOCK_WAIT_TICKS) {
                ReloadBuffer* dma_buffer = &sequence->dma_buffer;
                dma_buffer->read_pos =
                    RINGBUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2);
                FURI_LOG_D(
                    TAG,
                    "[SEQ] hung %lu ms in finish (ARR 0x%08lx, read %lu, write %lu)",
                    SEQ_LOCK_WAIT_MS,
                    TIM2->ARR,
                    dma_buffer->read_pos,
                    dma_buffer->write_pos);
                break;
            }
        } while(1);
    }

    digital_signal_stop_timer();
    digital_signal_stop_dma();
}

static void digital_sequence_queue_pulse(DigitalSequence* sequence, uint32_t length) {
    ReloadBuffer* dma_buffer = &sequence->dma_buffer;

    if(sequence->dma_active) {
        uint32_t prev_timer = DWT->CYCCNT;
        do {
            dma_buffer->read_pos = RINGBUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2);

            uint32_t free =
                (RINGBUFFER_SIZE + dma_buffer->read_pos - dma_buffer->write_pos) % RINGBUFFER_SIZE;

            if(free > 2) {
                break;
            }

            if(DWT->CYCCNT - prev_timer > SEQ_LOCK_WAIT_TICKS) {
                FURI_LOG_D(
                    TAG,
                    "[SEQ] hung %lu ms in queue (ARR 0x%08lx, read %lu, write %lu)",
                    SEQ_LOCK_WAIT_MS,
                    TIM2->ARR,
                    dma_buffer->read_pos,
                    dma_buffer->write_pos);
                break;
            }
            if(TIM2->ARR == SEQ_TIMER_MAX) {
                FURI_LOG_D(
                    TAG,
                    "[SEQ] buffer underrun in queue (ARR 0x%08lx, read %lu, write %lu)",
                    TIM2->ARR,
                    dma_buffer->read_pos,
                    dma_buffer->write_pos);
                break;
            }
        } while(1);
    }

    dma_buffer->buffer[dma_buffer->write_pos] = length;
    dma_buffer->write_pos++;
    dma_buffer->write_pos %= RINGBUFFER_SIZE;
    dma_buffer->buffer[dma_buffer->write_pos] = SEQ_TIMER_MAX;
}

bool digital_sequence_send(DigitalSequence* sequence) {
    furi_assert(sequence);

    ReloadBuffer* dma_buffer = &sequence->dma_buffer;

    // TODO: Initialise GPIO elsewhere
    furi_hal_gpio_init(sequence->gpio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
#ifdef DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN
    furi_hal_gpio_init(
        &DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
#endif

    if(!sequence->sequence_used) {
        return false;
    }

    int32_t remainder = 0;
    uint32_t trade_for_next = 0;
    uint32_t seq_pos_next = 1;

    sequence->dma_active = false;

    dma_buffer->buffer[0] = SEQ_TIMER_MAX;
    dma_buffer->read_pos = 0;
    dma_buffer->write_pos = 0;

    /* already prepare the current signal pointer */
    DigitalSignal* sig = sequence->signals[sequence->sequence[0]];
    DigitalSignal* sig_next = NULL;

    digital_sequence_setup_gpio(sequence, sig);

    FURI_CRITICAL_ENTER();

    while(sig) {
        bool last_signal = (seq_pos_next >= sequence->sequence_used);

        if(!last_signal) {
            sig_next = sequence->signals[sequence->sequence[seq_pos_next++]];
        }

        for(uint32_t pulse_pos = 0; pulse_pos < sig->edge_cnt; pulse_pos++) {
            bool last_pulse = ((pulse_pos + 1) >= sig->edge_cnt);
            uint32_t pulse_length = sig->reload_reg_buff[pulse_pos] + trade_for_next;

            trade_for_next = 0;

            /* when we are too late more than half a tick, make the first edge temporarily longer */
            if(remainder >= T_TIM_DIV2) {
                remainder -= T_TIM;
                pulse_length += 1;
            }

            if(last_pulse) {
                /* next signal is present */
                if(sig_next) {
                    /* when a signal ends with the same level as the next signal begins, let the next signal generate the whole pulse.
                    beware, we do not want the level after the last edge, but the last level before that edge */
                    bool end_level = sig->start_level ^ ((sig->edge_cnt % 2) == 0);

                    /* if they have the same level, pass the duration to the next pulse(s) */
                    if(end_level == sig_next->start_level) {
                        trade_for_next = pulse_length;
                    }
                } else {
                    /* if this is the last pulse of the last signal, ignore it */
                    trade_for_next = 1;
                }
            }

            /* if it was decided, that the next signal's first pulse shall also handle our "length", then do not queue here */
            if(trade_for_next == 0) {
                digital_sequence_queue_pulse(sequence, pulse_length);

                if(!sequence->dma_active) {
                    /* start transmission when buffer was filled enough */
                    bool start_send = sequence->dma_buffer.write_pos >= (RINGBUFFER_SIZE - 2);

                    /* or it was the last pulse */
                    if(last_pulse && last_signal) {
                        start_send = true;
                    }

                    /* start transmission */
                    if(start_send) {
                        digital_sequence_setup_dma(sequence);
                        digital_sequence_setup_timer();

                        /* if the send time is specified, wait till the core timer passed beyond that time */
                        if(sequence->send_time_active) {
                            sequence->send_time_active = false;
                            while(sequence->send_time - DWT->CYCCNT < 0x80000000) {
                            }
                        }
                        digital_sequence_start_timer();
                        sequence->dma_active = true;
                    }
                }
            }
        }

        remainder += sig->remainder;
        sig = sig_next;
        sig_next = NULL;
    }

    /* wait until last dma transaction was finished */
    FURI_CRITICAL_EXIT();
    digital_sequence_finish(sequence);

    return true;
}

void digital_sequence_clear(DigitalSequence* sequence) {
    furi_assert(sequence);

    sequence->sequence_used = 0;
}
