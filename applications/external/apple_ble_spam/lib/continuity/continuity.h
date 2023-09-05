#pragma once

#include <stdint.h>
#include <stdlib.h>

// Hacked together by @Willy-JL
// Structures docs and Nearby Action IDs from https://github.com/furiousMAC/continuity/
// Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/
// Custom adv logic and Airtag ID from https://techryptic.github.io/2023/09/01/Annoying-Apple-Fans/

typedef enum {
    ContinuityTypeNearbyAction,
    ContinuityTypeProximityPair,
    ContinuityTypeCount
} ContinuityType;

typedef union {
    struct {
        uint8_t type;
    } nearby_action;
    struct {
        uint8_t prefix;
        uint16_t model;
    } proximity_pair;
} ContinuityData;

typedef struct {
    ContinuityType type;
    ContinuityData data;
} ContinuityMsg;

const char* continuity_get_type_name(ContinuityType type);

size_t continuity_get_packet_size(ContinuityType type);

void continuity_generate_packet(const ContinuityMsg* msg, uint8_t* packet);
