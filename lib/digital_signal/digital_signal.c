#include "digital_signal_i.h"

#include <furi.h>
#include <furi_hal_bus.h>
#include <furi_hal_resources.h>

#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_tim.h>

#define TAG "DigitalSignal"

DigitalSignal* digital_signal_alloc(uint32_t max_edges_cnt) {
    DigitalSignal* signal = malloc(sizeof(DigitalSignal));

    signal->edges_max_cnt = max_edges_cnt;
    signal->reload_reg_buff = malloc(max_edges_cnt * sizeof(uint32_t));

    return signal;
}

void digital_signal_free(DigitalSignal* signal) {
    furi_assert(signal);

    free(signal->reload_reg_buff);
    free(signal);
}

// bool digital_signal_append(DigitalSignal* signal_a, DigitalSignal* signal_b) {
//     furi_assert(signal_a);
//     furi_assert(signal_b);
//
//     if(signal_a->edges_max_cnt < signal_a->edge_cnt + signal_b->edge_cnt) {
//         return false;
//     }
//     /* in case there are no edges in our target signal, the signal to append makes the rules */
//     if(!signal_a->edge_cnt) {
//         signal_a->start_level = signal_b->start_level;
//     }
//     bool end_level = signal_a->start_level;
//     if(signal_a->edge_cnt) {
//         end_level = signal_a->start_level ^ !(signal_a->edge_cnt % 2);
//     }
//     uint8_t start_copy = 0;
//     if(end_level == signal_b->start_level) {
//         if(signal_a->edge_cnt) {
//             signal_a->edge_timings[signal_a->edge_cnt - 1] += signal_b->edge_timings[0];
//             start_copy += 1;
//         } else {
//             signal_a->edge_timings[signal_a->edge_cnt] += signal_b->edge_timings[0];
//         }
//     }
//
//     for(size_t i = 0; i < signal_b->edge_cnt - start_copy; i++) {
//         signal_a->edge_timings[signal_a->edge_cnt + i] = signal_b->edge_timings[start_copy + i];
//     }
//     signal_a->edge_cnt += signal_b->edge_cnt - start_copy;
//
//     return true;
// }

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

    const uint32_t duration = ticks + signal->remainder;

    uint32_t reload_value = duration / T_TIM;
    int32_t remainder = duration - reload_value * T_TIM;

    if(remainder >= T_TIM_DIV2) {
        reload_value += 1;
        remainder -= T_TIM;
    }

    furi_check(reload_value > 1);

    signal->reload_reg_buff[signal->edge_cnt++] = reload_value - 1;
    signal->remainder = remainder;
}

static void digital_signal_extend_last_period(DigitalSignal* signal, uint32_t ticks) {
    const uint32_t reload_value_old = signal->reload_reg_buff[signal->edge_cnt - 1] + 1;
    const uint32_t duration = ticks + signal->remainder + reload_value_old * T_TIM;

    uint32_t reload_value = duration / T_TIM;
    int32_t remainder = duration - reload_value * T_TIM;

    if(remainder >= T_TIM_DIV2) {
        reload_value += 1;
        remainder -= T_TIM;
    }

    furi_check(reload_value > 1);

    signal->reload_reg_buff[signal->edge_cnt - 1] = reload_value - 1;
    signal->remainder = remainder;
}

void digital_signal_add_pulse(DigitalSignal* signal, uint32_t ticks, bool level) {
    furi_assert(signal);
    furi_assert(signal->edge_cnt < signal->edges_max_cnt);

    /* virgin signal? add it as the only level */
    if(signal->edge_cnt == 0) {
        signal->start_level = level;
        digital_signal_add_edge(signal, ticks);
    } else {
        bool end_level = signal->start_level ^ !(signal->edge_cnt % 2);

        if(level != end_level) {
            digital_signal_add_edge(signal, ticks);
        } else {
            digital_signal_extend_last_period(signal, ticks);
        }
    }
}
