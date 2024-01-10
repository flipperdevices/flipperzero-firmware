#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <gap.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FuriHalBleProfileConfig FuriHalBleProfileConfig;

/* Actual profiles must inherit (include this structure) as their first field */
typedef struct {
    /* Pointer to the config for this profile. Must be used to check if the
     * instance belongs to the profile */
    const FuriHalBleProfileConfig* config;
} FuriHalBleProfileBase;

typedef FuriHalBleProfileBase* (*FuriHalBleProfileStart)(void);
typedef void (*FuriHalBleProfileStop)(FuriHalBleProfileBase* profile);
typedef void (*FuriHalBleProfileGetGapConfig)(GapConfig* target_config);

struct FuriHalBleProfileConfig {
    /* Returns an instance of the profile */
    FuriHalBleProfileStart start;
    /* Destroys the instance of the profile.  Must check if instance belongs to the profile */
    FuriHalBleProfileStop stop;
    /* Called before starting the profile to get the GAP configuration */
    FuriHalBleProfileGetGapConfig get_gap_config;
};

#ifdef __cplusplus
}
#endif
