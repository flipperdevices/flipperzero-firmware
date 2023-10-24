#pragma once
#include "_base.h"

// Hacked together by @Willy-JL
// iOS 17 Crash by @ECTO-1A
// Nearby Action IDs and Documentation at https://github.com/furiousMAC/continuity/
// Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/

typedef enum {
    ContinuityTypeAirDrop = 0x05,
    ContinuityTypeProximityPair = 0x07,
    ContinuityTypeAirplayTarget = 0x09,
    ContinuityTypeHandoff = 0x0C,
    ContinuityTypeTetheringSource = 0x0E,
    ContinuityTypeNearbyAction = 0x0F,
    ContinuityTypeNearbyInfo = 0x10,

    ContinuityTypeCustomCrash,
    ContinuityTypeCOUNT
} ContinuityType;

typedef struct {
    ContinuityType type;
    union {
        struct {
            uint16_t model;
            uint8_t prefix;
        } proximity_pair;
        struct {
            uint8_t action;
            uint8_t flags;
        } nearby_action;
    } data;
} ContinuityCfg;

extern const Protocol protocol_continuity;
