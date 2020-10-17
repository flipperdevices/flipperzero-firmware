#pragma once
#include "flipper.h"

static inline __attribute__((always_inline)) void ir_on() {
    HAL_GPIO_WritePin(IR_TX_GPIO_Port, IR_TX_Pin, GPIO_PIN_SET);
}

static inline __attribute__((always_inline)) void ir_off() {
    HAL_GPIO_WritePin(IR_TX_GPIO_Port, IR_TX_Pin, GPIO_PIN_RESET);
}