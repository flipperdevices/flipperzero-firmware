#include "digital_sequence.h"
#include "digital_signal_i.h"

#include <furi.h>
#include <furi_hal_bus.h>

#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_tim.h>

/* must be on bank B */
// For debugging purposes use `--extra-define=DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN=gpio_ext_pb3` fbt option

#define TAG "DigitalSequence"

/* end marker in DMA ringbuffer, will get written into timer register at the end */
#define DIGITAL_SEQUENCE_TIMER_MAX 0xFFFFFFFFUL

/* time to wait in loops before returning */
#define DIGITAL_SEQUENCE_LOCK_WAIT_MS 10UL
#define DIGITAL_SEQUENCE_LOCK_WAIT_TICKS (DIGITAL_SEQUENCE_LOCK_WAIT_MS * 1000 * 64)

#define DIGITAL_SEQUENCE_GPIO_BUFFER_SIZE 2

/* maximum entry count of the sequence dma ring buffer */
#define DIGITAL_SEQUENCE_RING_BUFFER_SIZE 128

#define DIGITAL_SEQUENCE_RING_BUFFER_MIN_FREE_SIZE 2

/* maximum number of DigitalSignals in a sequence */
#define DIGITAL_SEQUENCE_BANK_SIZE 32

typedef enum {
    DigitalSequenceStateIdle,
    DigitalSequenceStateActive,
} DigitalSequenceState;

typedef struct {
    uint32_t data[DIGITAL_SEQUENCE_RING_BUFFER_SIZE]; /* DMA ringbuffer */
    uint32_t write_pos; /* current buffer write index */
    uint32_t read_pos; /* current buffer read index */
} DigitalSequenceRingBuffer;

typedef uint32_t DigitalSequenceGpioBuffer[DIGITAL_SEQUENCE_GPIO_BUFFER_SIZE];

typedef DigitalSignal* DigitalSequenceSignalBank[DIGITAL_SEQUENCE_BANK_SIZE];

struct DigitalSequence {
    const GpioPin* gpio;

    uint32_t size;
    uint32_t max_size;
    uint8_t* data;

    LL_DMA_InitTypeDef dma_config_gpio;
    LL_DMA_InitTypeDef dma_config_timer;

    DigitalSequenceGpioBuffer gpio_buf;
    DigitalSequenceRingBuffer timer_buf;
    DigitalSequenceSignalBank signals;
    DigitalSequenceState state;
};

DigitalSequence* digital_sequence_alloc(uint32_t size, const GpioPin* gpio) {
    furi_assert(size);
    furi_assert(gpio);

    DigitalSequence* sequence = malloc(sizeof(DigitalSequence));

    sequence->gpio = gpio;
    sequence->max_size = size;

    sequence->data = malloc(sequence->max_size);

    sequence->dma_config_gpio.PeriphOrM2MSrcAddress = (uint32_t)&gpio->port->BSRR;
    sequence->dma_config_gpio.MemoryOrM2MDstAddress = (uint32_t)sequence->gpio_buf;
    sequence->dma_config_gpio.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    sequence->dma_config_gpio.Mode = LL_DMA_MODE_CIRCULAR;
    sequence->dma_config_gpio.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    sequence->dma_config_gpio.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    sequence->dma_config_gpio.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    sequence->dma_config_gpio.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    sequence->dma_config_gpio.NbData = DIGITAL_SEQUENCE_GPIO_BUFFER_SIZE;
    sequence->dma_config_gpio.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    sequence->dma_config_gpio.Priority = LL_DMA_PRIORITY_VERYHIGH;

    sequence->dma_config_timer.PeriphOrM2MSrcAddress = (uint32_t)&TIM2->ARR;
    sequence->dma_config_timer.MemoryOrM2MDstAddress = (uint32_t)sequence->timer_buf.data;
    sequence->dma_config_timer.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    sequence->dma_config_timer.Mode = LL_DMA_MODE_CIRCULAR;
    sequence->dma_config_timer.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    sequence->dma_config_timer.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    sequence->dma_config_timer.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    sequence->dma_config_timer.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    sequence->dma_config_timer.NbData = DIGITAL_SEQUENCE_RING_BUFFER_SIZE;
    sequence->dma_config_timer.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    sequence->dma_config_timer.Priority = LL_DMA_PRIORITY_HIGH;

    return sequence;
}

void digital_sequence_free(DigitalSequence* sequence) {
    furi_assert(sequence);

    free(sequence->data);
    free(sequence);
}

void digital_sequence_register_signal(
    DigitalSequence* sequence,
    uint8_t signal_index,
    DigitalSignal* signal) {
    furi_assert(sequence);
    furi_assert(signal);
    furi_assert(signal_index < DIGITAL_SEQUENCE_BANK_SIZE);

    sequence->signals[signal_index] = signal;
}

void digital_sequence_add_signal(DigitalSequence* sequence, uint8_t signal_index) {
    furi_assert(sequence);
    furi_assert(signal_index < DIGITAL_SEQUENCE_BANK_SIZE);
    furi_assert(sequence->size < sequence->max_size);

    sequence->data[sequence->size++] = signal_index;
}

static inline void digital_sequence_start_dma(DigitalSequence* sequence) {
    furi_assert(sequence);

    /* set up DMA channel 1 and 2 for GPIO and timer copy operations */
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &sequence->dma_config_gpio);
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_2, &sequence->dma_config_timer);

    /* enable both DMA channels */
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
}

static inline void digital_sequence_stop_dma() {
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ClearFlag_TC2(DMA1);
}

static inline void digital_sequence_start_timer() {
    furi_hal_bus_enable(FuriHalBusTIM2);

    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    LL_TIM_SetAutoReload(TIM2, DIGITAL_SEQUENCE_TIMER_MAX);
    LL_TIM_SetCounter(TIM2, 0);

    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableUpdateEvent(TIM2);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);
    LL_TIM_GenerateEvent_UPDATE(TIM2);
}

static void digital_sequence_stop_timer() {
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_DisableUpdateEvent(TIM2);
    LL_TIM_DisableDMAReq_UPDATE(TIM2);

    furi_hal_bus_disable(FuriHalBusTIM2);
}

static inline void digital_sequence_init_gpio_buffer(
    DigitalSequence* sequence,
    const DigitalSignal* first_signal) {
    const uint32_t bit_set = sequence->gpio->pin << GPIO_BSRR_BS0_Pos
#ifdef DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN
                             | DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN.pin << GPIO_BSRR_BS0_Pos
#endif
        ;

    const uint32_t bit_reset = sequence->gpio->pin << GPIO_BSRR_BR0_Pos
#ifdef DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN
                               | DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN.pin << GPIO_BSRR_BR0_Pos
#endif
        ;

    if(first_signal->start_level) {
        sequence->gpio_buf[0] = bit_set;
        sequence->gpio_buf[1] = bit_reset;
    } else {
        sequence->gpio_buf[0] = bit_reset;
        sequence->gpio_buf[1] = bit_set;
    }
}

static inline void digital_sequence_finish(DigitalSequence* sequence) {
    if(sequence->state == DigitalSequenceStateActive) {
        const uint32_t prev_timer = DWT->CYCCNT;

        do {
            /* we are finished, when the DMA transferred the DIGITAL_SEQUENCE_TIMER_MAX marker */
            if(TIM2->ARR == DIGITAL_SEQUENCE_TIMER_MAX) {
                break;
            }

            if(DWT->CYCCNT - prev_timer > DIGITAL_SEQUENCE_LOCK_WAIT_TICKS) {
                DigitalSequenceRingBuffer* dma_buffer = &sequence->timer_buf;
                dma_buffer->read_pos = DIGITAL_SEQUENCE_RING_BUFFER_SIZE -
                                       LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2);
                FURI_LOG_D(
                    TAG,
                    "[SEQ] hung %lu ms in finish (ARR 0x%08lx, read %lu, write %lu)",
                    DIGITAL_SEQUENCE_LOCK_WAIT_MS,
                    TIM2->ARR,
                    dma_buffer->read_pos,
                    dma_buffer->write_pos);
                break;
            }
        } while(true);
    }

    digital_sequence_stop_timer();
    digital_sequence_stop_dma();
}

static inline void digital_sequence_enqueue_period(DigitalSequence* sequence, uint32_t length) {
    DigitalSequenceRingBuffer* dma_buffer = &sequence->timer_buf;

    if(sequence->state == DigitalSequenceStateActive) {
        const uint32_t prev_timer = DWT->CYCCNT;

        do {
            dma_buffer->read_pos =
                DIGITAL_SEQUENCE_RING_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2);

            const uint32_t size_free = (DIGITAL_SEQUENCE_RING_BUFFER_SIZE + dma_buffer->read_pos -
                                        dma_buffer->write_pos) %
                                       DIGITAL_SEQUENCE_RING_BUFFER_SIZE;

            if(size_free > DIGITAL_SEQUENCE_RING_BUFFER_MIN_FREE_SIZE) {
                break;
            }

            if(DWT->CYCCNT - prev_timer > DIGITAL_SEQUENCE_LOCK_WAIT_TICKS) {
                FURI_LOG_D(
                    TAG,
                    "[SEQ] hung %lu ms in queue (ARR 0x%08lx, read %lu, write %lu)",
                    DIGITAL_SEQUENCE_LOCK_WAIT_MS,
                    TIM2->ARR,
                    dma_buffer->read_pos,
                    dma_buffer->write_pos);
                break;
            }

            if(TIM2->ARR == DIGITAL_SEQUENCE_TIMER_MAX) {
                FURI_LOG_D(
                    TAG,
                    "[SEQ] buffer underrun in queue (ARR 0x%08lx, read %lu, write %lu)",
                    TIM2->ARR,
                    dma_buffer->read_pos,
                    dma_buffer->write_pos);
                break;
            }
        } while(true);
    }

    dma_buffer->data[dma_buffer->write_pos] = length;

    dma_buffer->write_pos += 1;
    dma_buffer->write_pos %= DIGITAL_SEQUENCE_RING_BUFFER_SIZE;

    dma_buffer->data[dma_buffer->write_pos] = DIGITAL_SEQUENCE_TIMER_MAX;
}

static inline void digital_sequence_timer_buffer_reset(DigitalSequence* sequence) {
    sequence->timer_buf.data[0] = DIGITAL_SEQUENCE_TIMER_MAX;
    sequence->timer_buf.read_pos = 0;
    sequence->timer_buf.write_pos = 0;
}

void digital_sequence_transmit(DigitalSequence* sequence) {
    furi_assert(sequence);
    furi_assert(sequence->size);
    furi_assert(sequence->state == DigitalSequenceStateIdle);

    FURI_CRITICAL_ENTER();

    furi_hal_gpio_init(sequence->gpio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
#ifdef DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN
    furi_hal_gpio_init(
        &DIGITAL_SIGNAL_DEBUG_OUTPUT_PIN, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
#endif

    /* already prepare the current signal pointer */
    const DigitalSignal* signal_current = sequence->signals[sequence->data[0]];

    digital_sequence_init_gpio_buffer(sequence, signal_current);

    int32_t remainder_ticks = 0;
    uint32_t reload_value_carry = 0;
    uint32_t next_idx = 1;

    do {
        const DigitalSignal* signal_next =
            (next_idx < sequence->size) ? sequence->signals[sequence->data[next_idx++]] : NULL;

        for(uint32_t i = 0; i < signal_current->size; i++) {
            const bool is_last_value = (i == signal_current->size - 1);
            uint32_t reload_value = signal_current->data[i] + reload_value_carry;

            reload_value_carry = 0;

            /* when we are too late more than half a tick, make the first period temporarily longer */
            if(remainder_ticks >= DIGITAL_SIGNAL_T_TIM_DIV2) {
                remainder_ticks -= DIGITAL_SIGNAL_T_TIM;
                reload_value += 1;
            }

            if(is_last_value) {
                /* next signal is present */
                if(signal_next) {
                    /* when a signal ends with the same level as the next signal begins, let the next signal generate the whole pulse.
                    beware, we do not want the level after the last edge, but the last level before that edge */
                    const bool end_level = signal_current->start_level ^
                                           ((signal_current->size % 2) == 0);

                    /* if they have the same level, pass the duration to the next pulse(s) */
                    if(end_level == signal_next->start_level) {
                        reload_value_carry = reload_value;
                    }
                } else {
                    /* if this is the last pulse of the last signal, ignore it */
                    reload_value_carry = 1;
                }
            }

            /* if it was decided, that the next signal's first pulse shall also handle our "length", then do not queue here */
            if(reload_value_carry == 0) {
                digital_sequence_enqueue_period(sequence, reload_value);

                if(sequence->state == DigitalSequenceStateIdle) {
                    const bool is_buffer_filled = sequence->timer_buf.write_pos >=
                                                  (DIGITAL_SEQUENCE_RING_BUFFER_SIZE -
                                                   DIGITAL_SEQUENCE_RING_BUFFER_MIN_FREE_SIZE);
                    const bool can_start = is_buffer_filled || (!signal_next && is_last_value);

                    if(can_start) {
                        digital_sequence_start_dma(sequence);
                        digital_sequence_start_timer();
                        sequence->state = DigitalSequenceStateActive;
                    }
                }
            }
        }

        remainder_ticks += signal_current->remainder;
        signal_current = signal_next;

    } while(signal_current);

    digital_sequence_finish(sequence);
    digital_sequence_timer_buffer_reset(sequence);

    FURI_CRITICAL_EXIT();

    sequence->state = DigitalSequenceStateIdle;
}

void digital_sequence_clear(DigitalSequence* sequence) {
    furi_assert(sequence);

    sequence->size = 0;
}
