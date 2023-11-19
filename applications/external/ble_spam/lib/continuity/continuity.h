#pragma once

#include <stdint.h>
#include <stdlib.h>

// Hacked together by @Willy-JL
// Custom adv logic by @Willy-JL (idea by @xMasterX)
// iOS 17 Crash by @ECTO-1A
// Extensive testing and research on behavior and parameters by @Willy-JL and @ECTO-1A
// Structures docs and Nearby Action IDs from https://github.com/furiousMAC/continuity/
// Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/
// Controversy explained at https://willyjl.dev/blog/the-controversy-behind-apple-ble-spam

typedef enum {
    ContinuityTypeAirDrop = 0x05,
    ContinuityTypeProximityPair = 0x07,
    ContinuityTypeAirplayTarget = 0x09,
    ContinuityTypeHandoff = 0x0C,
    ContinuityTypeTetheringSource = 0x0E,
    ContinuityTypeNearbyAction = 0x0F,
    ContinuityTypeNearbyInfo = 0x10,

    ContinuityTypeCustomCrash,
    ContinuityTypeCount
} ContinuityType;

typedef union {
    struct {
    } airdrop;
    struct {
        uint8_t prefix;
        uint16_t model;
    } proximity_pair;
    struct {
    } airplay_target;
    struct {
    } handoff;
    struct {
    } tethering_source;
    struct {
        uint8_t flags;
        uint8_t type;
    } nearby_action;
    struct {
    } nearby_info;
    struct {
    } custom_crash;
} ContinuityData;

typedef struct {
    ContinuityType type;
    ContinuityData data;
} ContinuityMsg;

const char* continuity_get_type_name(ContinuityType type);

uint8_t continuity_get_packet_size(ContinuityType type);

void continuity_generate_packet(const ContinuityMsg* msg, uint8_t* packet);
