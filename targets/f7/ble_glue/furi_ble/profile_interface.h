#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <gap.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FuriHalBleProfileConfig FuriHalBleProfileConfig;

// Actual profiles must inherit (include this structure) as their first field
typedef struct {
    const FuriHalBleProfileConfig* config;
} FuriHalBleProfileBase;

typedef FuriHalBleProfileBase* (*FuriHalBleProfileStart)(void);
typedef void (*FuriHalBleProfileStop)(FuriHalBleProfileBase* profile);
typedef void (*FuriHalBleProfileGetGapConfig)(GapConfig* target_config);

struct FuriHalBleProfileConfig {
    FuriHalBleProfileStart start;
    FuriHalBleProfileStop stop;
    FuriHalBleProfileGetGapConfig get_gap_config;
};

#ifdef __cplusplus
}
#endif
