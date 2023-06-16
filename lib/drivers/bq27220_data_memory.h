#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BQ27220DMTypeEnd,
    BQ27220DMTypeWait,
    BQ27220DMTypeU8,
    BQ27220DMTypeU16,
    BQ27220DMTypeU32,
    BQ27220DMTypeF32,
} BQ27220DMType;

typedef enum {
    BQ27220DMAddressGasGaugingCEDVProfile1GaugingConfig = 0x929B,
    BQ27220DMAddressGasGaugingCEDVProfile1FullChargeCapacity = 0x929D,
    BQ27220DMAddressGasGaugingCEDVProfile1DesignCapacity = 0x929F,
    BQ27220DMAddressGasGaugingCEDVProfile1EMF = 0x92A3,
    BQ27220DMAddressGasGaugingCEDVProfile1C0 = 0x92A9,
    BQ27220DMAddressGasGaugingCEDVProfile1R0 = 0x92AB,
    BQ27220DMAddressGasGaugingCEDVProfile1T0 = 0x92AD,
    BQ27220DMAddressGasGaugingCEDVProfile1R1 = 0x92AF,
    BQ27220DMAddressGasGaugingCEDVProfile1TC = 0x92B1,
    BQ27220DMAddressGasGaugingCEDVProfile1C1 = 0x92B2,
    BQ27220DMAddressGasGaugingCEDVProfile1EDV0 = 0x92B4,
    BQ27220DMAddressGasGaugingCEDVProfile1EDV1 = 0x92B7,
    BQ27220DMAddressGasGaugingCEDVProfile1EDV2 = 0x92BA,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD0 = 0x92BD,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD10 = 0x92BF,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD20 = 0x92C1,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD30 = 0x92C3,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD40 = 0x92C5,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD50 = 0x92C7,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD60 = 0x92C9,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD70 = 0x92CB,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD80 = 0x92CD,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD90 = 0x92CF,
    BQ27220DMAddressGasGaugingCEDVProfile1StartDOD100 = 0x92D1,
    BQ27220DMAddressCalibrationCurrentDeadband = 0x91DE,
    BQ27220DMAddressConfigurationPowerSleepCurrent = 0x9217,
    BQ27220DMAddressConfigurationCurrentThresholdsDischargeDetectionThreshold = 0x9228,
} BQ27220DMAddress;

typedef struct BQ27220DMData BQ27220DMData;

struct BQ27220DMData {
    uint16_t type;
    uint16_t address;
    union {
        uint8_t u8;
        uint16_t u16;
        uint16_t u32;
        float f32;
    } value;
};
