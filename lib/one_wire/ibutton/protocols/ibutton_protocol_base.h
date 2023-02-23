#pragma once

typedef void iButtonProtocolData;

typedef enum {
    iButtonProtocolFeatureExtData = (1U << 0),
    iButtonProtocolFeatureWriteBlank = (1U << 1),
    iButtonProtocolFeatureWriteCopy = (1U << 2),
    iButtonProtocolFeatureApplyEdits = (1U << 3),
} iButtonProtocolFeature;
