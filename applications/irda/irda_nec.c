#include "irda_nec.h"
#include "irda.h"

// carrier freq = 38kHz
// carrier timing: 1000 / 38 = 26,315
// with rounding and pin toggling overhead i get 12+12 us delay
#define NEC_CARRIER_FREQUENCY 38000
#define NEC_CARRIER_PERIOD (1000000.0 / (float)NEC_CARRIER_FREQUENCY)
#define NEC_PIN_UP_US 12
#define NEC_PIN_DOWN_US 12

static inline __attribute__((always_inline)) void ir_nec_pulse() {
    ir_on();
    delay_us(NEC_PIN_UP_US);
    ir_off();
    delay_us(NEC_PIN_DOWN_US);
}

void ir_nec_preambula(void) {
    // 9ms carrier + 4.5ms pause
    const uint16_t periods_in_9ms = (9000.0 / NEC_CARRIER_PERIOD);
    for(uint16_t i = 0; i < periods_in_9ms; i++) {
        ir_nec_pulse();
    }
    delay_us(4500);
}

void ir_nec_send_bit(bool bit) {
    // 0 is 562.5us carrier + 1687.5us pause
    // 1 is 562.5us carrier + 562.5us pause
    const uint16_t periods_in_562_5us = (562.5 / NEC_CARRIER_PERIOD);
    for(uint16_t i = 0; i < periods_in_562_5us; i++) {
        ir_nec_pulse();
    }
    if(bit) {
        delay_us(562.5);
    } else {
        delay_us(1687.5);
    }
}

void ir_nec_send_byte(uint8_t data) {
    for(uint8_t i = 0; i < 8; i++) {
        ir_nec_send_bit((data & (1 << (i))) != 0);
    }
}

void ir_nec_send(uint16_t addr, uint8_t data) {
    // nec protocol is:
    // preambula + addr high + addr low + command + inverse command + bit pulse
    //
    // oddly enough, my analyzer (https://github.com/ukw100/IRMP) displays the reverse command
    // and I don’t know if this is my fault or a feature of the analyzer
    // TODO: check the dictionary and check with a known remote
    uint8_t nec_packet[4] = {addr >> 8, addr, ~(uint8_t)data, data};
    ir_nec_preambula();
    ir_nec_send_byte(nec_packet[0]);
    ir_nec_send_byte(nec_packet[1]);
    ir_nec_send_byte(nec_packet[2]);
    ir_nec_send_byte(nec_packet[3]);
    ir_nec_send_bit(1);
}