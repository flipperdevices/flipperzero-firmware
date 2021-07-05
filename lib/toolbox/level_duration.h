#pragma once

#include <stdint.h>

#define LEVEL_DURATION_RESET 0U
#define LEVEL_DURATION_RESERVED 0x800000U

typedef int32_t LevelDuration;

static inline bool level_duration_is_reset(LevelDuration level_duration) {
    return (level_duration == LEVEL_DURATION_RESET);
}

static inline bool level_duration_get_level(LevelDuration level_duration) {
    return (level_duration > 0);
}

static inline uint32_t level_duration_get_duration(LevelDuration level_duration) {
    return (level_duration >= 0) ? level_duration : -level_duration;
}
