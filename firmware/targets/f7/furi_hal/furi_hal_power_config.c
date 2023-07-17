#include <bq27220_data_memory.h>

// typedef struct {
//     // Low byte, Low bit first
//     bool CCT : 1;
//     bool CSYNC : 1;
//     bool RSVD0 : 1;
//     bool EDV_CMP : 1;
//     bool SC : 1;
//     bool FIXED_EDV0 : 1;
//     uint8_t RSVD1 : 2;
//     // High byte, Low bit first
//     bool FCC_LIM : 1;
//     bool RSVD2 : 1;
//     bool FC_FOR_VDQ : 1;
//     bool IGNORE_SD : 1;
//     bool SME0 : 1;
//     uint8_t RSVD3 : 3;
// } GaugingConfig;

// _Static_assert(sizeof(GaugingConfig) == 2, "Incorrect structure size");

// const GaugingConfig gauge_conf = {
//     .CCT = 1,
//     .CSYNC = 0,
//     .EDV_CMP = 0,
//     .SC = 1,
//     .FIXED_EDV0 = 1,
//     .FCC_LIM = 1,
//     .FC_FOR_VDQ = 1,
//     .IGNORE_SD = 1,
//     .SME0 = 0,
// };

const BQ27220DMData furi_hal_power_gauge_data_memory[] = {
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1GaugingConfig,
        .type = BQ27220DMTypeU16,
        .value.u16 = 0b1011000010001100,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1FullChargeCapacity,
        .type = BQ27220DMTypeU16,
        .value.u16 = 2101,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1DesignCapacity,
        .type = BQ27220DMTypeU16,
        .value.u16 = 2101,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1EMF,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3679,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1C0,
        .type = BQ27220DMTypeU16,
        .value.u16 = 430,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1R0,
        .type = BQ27220DMTypeU16,
        .value.u16 = 334,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1T0,
        .type = BQ27220DMTypeU16,
        .value.u16 = 4626,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1R1,
        .type = BQ27220DMTypeU16,
        .value.u16 = 408,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1TC,
        .type = BQ27220DMTypeU8,
        .value.u8 = 11,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1C1,
        .type = BQ27220DMTypeU8,
        .value.u8 = 0,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD0,
        .type = BQ27220DMTypeU16,
        .value.u16 = 4044,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD10,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3905,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD20,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3807,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD30,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3718,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD40,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3642,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD50,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3585,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD60,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3546,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD70,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3514,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD80,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3477,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD90,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3411,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1StartDOD100,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3299,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1EDV0,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3300,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1EDV1,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3321,
    },
    {
        .address = BQ27220DMAddressGasGaugingCEDVProfile1EDV2,
        .type = BQ27220DMTypeU16,
        .value.u16 = 3355,
    },
    {
        .type = BQ27220DMTypeEnd,
    },
};
