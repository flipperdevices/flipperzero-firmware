#pragma once

#include <infrared_transmit.h>
#include <infrared_worker.h>

#include "furi_hal.h"

#define HVAC_MITSUBISHI_HDR_MARK 3400
#define HVAC_MITSUBISHI_HDR_SPACE 1750
#define HVAC_MITSUBISHI_BIT_MARK 450
#define HVAC_MITSUBISHI_ONE_SPACE 1300
#define HVAC_MITSUBISHI_ZERO_SPACE 420
#define HVAC_MITSUBISHI_RPT_MARK 440
#define HVAC_MITSUBISHI_RPT_SPACE 17100

#define HVAC_MITSUBISHI_PACKET_SIZE 18

#define HVAC_MITSUBISHI_TEMPERATURE_MIN 16
#define HVAC_MITSUBISHI_TEMPERATURE_MAX 31

typedef enum {
    HvacMitsubishiPowerOn,
    HvacMitsubishiPowerOff,
} HvacMitsubishiPower;

typedef enum {
    HvacMitsubishiModeHeat,
    HvacMitsubishiModeCold,
    HvacMitsubishiModeDry,
    HvacMitsubishiModeAuto,
} HvacMitsubishiMode;

typedef enum {
    HvacMitsubishiFanSpeedAuto,
    HvacMitsubishiFanSpeed1,
    HvacMitsubishiFanSpeed2,
    HvacMitsubishiFanSpeed3,
    HvacMitsubishiFanSpeed4,
    HvacMitsubishiFanSpeedSilent,
} HvacMitsubishiFanSpeed;

typedef enum {
    HvacMitsubishiVaneAuto,
    HvacMitsubishiVaneH1,
    HvacMitsubishiVaneH2,
    HvacMitsubishiVaneH3,
    HvacMitsubishiVaneH4,
    HvacMitsubishiVaneH5,
    HvacMitsubishiVaneAutoMove,
} HvacMitsubishiVane;

uint8_t* hvac_mitsubishi_init();
void hvac_mitsubishi_deinit(uint8_t* packet);
void hvac_mitsubishi_power(uint8_t* packet, HvacMitsubishiPower power);
void hvac_mitsubishi_set_mode(uint8_t* packet, HvacMitsubishiMode mode);
void hvac_mitsubishi_set_fan_speed(uint8_t* packet, HvacMitsubishiFanSpeed speed);
void hvac_mitsubishi_set_temperature(uint8_t* packet, uint8_t temp);
void hvac_mitsubishi_set_vane(uint8_t* packet, HvacMitsubishiVane mode);
void hvac_mitsubishi_send(uint8_t* packet);