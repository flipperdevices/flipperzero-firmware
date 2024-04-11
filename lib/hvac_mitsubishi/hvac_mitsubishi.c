/*
Protocol description:
https://github.com/r45635/HVAC-IR-Control/blob/4b6b7944b28ce78247f19744c272a36935bbb305/Protocol/Mitsubishi_IR_data_Data_v1.1-FULL.pdf

Used code:
https://github.com/r45635/HVAC-IR-Control/blob/4b6b7944b28ce78247f19744c272a36935bbb305/HVACDemo/IRremote2.cpp
*/

#include "hvac_mitsubishi.h"
#define TAG "hvac_mitsubishi"

static uint8_t HVAC_MITSUBISHI_DATA[18] = {
    0x23,
    0xCB,
    0x26,
    0x01,
    0x00,
    0b00000000, // off
    0b00100000, // mode_auto
    0b00000111, // 23
    0b00110000, // mode_auto
    0b11000000, // ac_fan_speed_auto + ac_vane_auto
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00};

uint8_t* hvac_mitsubishi_init() {
    uint8_t* data = malloc(sizeof(HVAC_MITSUBISHI_DATA));
    memcpy(data, HVAC_MITSUBISHI_DATA, sizeof(HVAC_MITSUBISHI_DATA));
    return data;
}

void hvac_mitsubishi_deinit(uint8_t* data) {
    free(data);
}

void hvac_mitsubishi_power(uint8_t* data, HvacMitsubishiPower power) {
    switch(power) {
    case HvacMitsubishiPowerOn:
        data[5] = data[5] | 0b00100000;
        break;
    case HvacMitsubishiPowerOff:
        data[5] = data[5] & 0b11011111;
        break;
    default:
        break;
    }
}

void hvac_mitsubishi_set_mode(uint8_t* data, HvacMitsubishiMode mode) {
    data[6] = data[6] & 0b11000111;
    data[8] = data[8] & 0b11111000;
    switch(mode) {
    case HvacMitsubishiModeHeat:
        data[6] = data[6] | 0b00001000;
        break;
    case HvacMitsubishiModeCold:
        data[6] = data[6] | 0b00011000;
        data[8] = data[8] | 0b00000110;
        break;
    case HvacMitsubishiModeDry:
        data[6] = data[6] | 0b00010000;
        data[8] = data[8] | 0b00000010;
        break;
    case HvacMitsubishiModeAuto:
        data[6] = data[6] | 0b00100000;
        break;
    default:
        break;
    }
}

void hvac_mitsubishi_set_temperature(uint8_t* data, uint8_t temp) {
    uint8_t cur_temp;
    if(temp > HVAC_MITSUBISHI_TEMPERATURE_MAX) {
        cur_temp = HVAC_MITSUBISHI_TEMPERATURE_MAX;
    } else if(temp < HVAC_MITSUBISHI_TEMPERATURE_MIN) {
        cur_temp = HVAC_MITSUBISHI_TEMPERATURE_MIN;
    } else {
        cur_temp = temp;
    };
    data[7] = (data[7] & 0b11110000) | (cur_temp - 16);
}

void hvac_mitsubishi_set_fan_speed(uint8_t* data, HvacMitsubishiFanSpeed speed) {
    data[9] = data[9] & 0b01111000;
    switch(speed) {
    case HvacMitsubishiFanSpeed1:
        data[9] = data[9] | 0b00000001;
        break;
    case HvacMitsubishiFanSpeed2:
        data[9] = data[9] | 0b00000010;
        break;
    case HvacMitsubishiFanSpeed3:
        data[9] = data[9] | 0b00000011;
        break;
    case HvacMitsubishiFanSpeed4:
        data[9] = data[9] | 0b00000100;
        break;
    case HvacMitsubishiFanSpeedAuto:
        data[9] = data[9] | 0b10000000;
        break;
    case HvacMitsubishiFanSpeedSilent:
        data[9] = data[9] | 0b00000101;
        break;
    default:
        break;
    }
}

void hvac_mitsubishi_set_vane(uint8_t* data, HvacMitsubishiVane mode) {
    data[9] = data[9] & 0b10000111;
    switch(mode) {
    case HvacMitsubishiVaneAuto:
        data[9] = data[9] | 0b01000000;
        break;
    case HvacMitsubishiVaneH1:
        data[9] = data[9] | 0b01001000;
        break;
    case HvacMitsubishiVaneH2:
        data[9] = data[9] | 0b01010000;
        break;
    case HvacMitsubishiVaneH3:
        data[9] = data[9] | 0b01011000;
        break;
    case HvacMitsubishiVaneH4:
        data[9] = data[9] | 0b01100000;
        break;
    case HvacMitsubishiVaneH5:
        data[9] = data[9] | 0b01101000;
        break;
    case HvacMitsubishiVaneAutoMove:
        data[9] = data[9] | 0b01111000;
        break;
    default:
        break;
    }
}

void hvac_mitsubishi_checksum(uint8_t* data) {
    data[17] = 0;
    for(int i = 0; i < 17; i++) {
        data[17] = data[i] + data[17]; // CRC is a simple bits addition
    }
}

void hvac_mitsubishi_send(uint8_t* data) {
    hvac_mitsubishi_checksum(data);
    char mask = 1;
    uint32_t* timings = malloc(sizeof(uint32_t) * MAX_TIMINGS_AMOUNT);
    uint32_t frequency = 38000;
    float duty_cycle = 0.33;
    size_t timings_size = 0;
    int j = 0;
    if((int)sizeof(data) / (int)sizeof(data[0]) + 8 < MAX_TIMINGS_AMOUNT) {
        while(j < 2) {
            timings[timings_size] = HVAC_MITSUBISHI_HDR_MARK;
            timings_size++;
            timings[timings_size] = HVAC_MITSUBISHI_HDR_SPACE;
            timings_size++;
            for(int i = 0; i < 18; i++) {
                for(mask = 00000001; mask > 0; mask <<= 1) {
                    if(data[i] & mask) {
                        timings[timings_size] = HVAC_MITSUBISHI_BIT_MARK;
                        timings_size++;
                        timings[timings_size] = HVAC_MITSUBISHI_ONE_SPACE;
                        timings_size++;
                    } else {
                        timings[timings_size] = HVAC_MITSUBISHI_BIT_MARK;
                        timings_size++;
                        timings[timings_size] = HVAC_MITSUBISHI_ZERO_SPACE;
                        timings_size++;
                    }
                }
            }
            timings[timings_size] = HVAC_MITSUBISHI_RPT_MARK;
            timings_size++;
            timings[timings_size] = HVAC_MITSUBISHI_RPT_SPACE;
            timings_size++;
            j++;
        }
    }
    infrared_send_raw_ext(timings, timings_size, true, frequency, duty_cycle);
    free(timings);
}
