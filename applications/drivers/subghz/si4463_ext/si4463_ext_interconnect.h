#pragma once
#include <lib/subghz/devices/types.h>
#include "driver/si446x_regs.h"

#define SUBGHZ_DEVICE_SI4463_EXT_NAME "si4463_ext"

typedef struct SubGhzDeviceSi4463Ext SubGhzDeviceSi4463Ext;

const FlipperAppPluginDescriptor* subghz_device_si4463_ext_ep();

typedef enum {
    SubGhzDeviceIOCTL_SI4463GetProperties = 0x10000000,
    SubGhzDeviceIOCTL_SI4463SetProperties,
    SubGhzDeviceIOCTL_SI4463SetPath,
} SubGhzDeviceIOCTL_SI4463;

typedef struct {
    SI446X_Prop_t prop;
    uint8_t data[16];
    uint8_t size;
} SubGhzDeviceIOCTL_SI4463GetPropertiesData;

typedef struct {
    SI446X_Prop_t prop;
    uint8_t data[12];
    uint8_t size;
} SubGhzDeviceIOCTL_SI4463SetPropertiesData;

typedef enum {
    SubGhzDeviceIOCTL_Si4463ExtPathIsolate = 0,
    SubGhzDeviceIOCTL_Si4463ExtPath433 = 433,
    SubGhzDeviceIOCTL_Si4463ExtPath315 = 315,
    SubGhzDeviceIOCTL_Si4463ExtPath868 = 868,
} SubGhzDeviceIOCTL_Si4463ExtPath;

typedef struct {
    SubGhzDeviceIOCTL_Si4463ExtPath path;
} SubGhzDeviceIOCTL_SI4463SetPathData;